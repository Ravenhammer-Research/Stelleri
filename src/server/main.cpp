/* netd main: initialize libnetconf2, wire logging and run a simple loop.
 * This keeps protocol-specific handlers in `Server`.
 */

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf build only"
#endif

#include "Logger.hpp"
#include "Server.hpp"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#endif
#include <libnetconf2/log.h>
#include <libnetconf2/server_config.h>
#include <libnetconf2/session_server.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstring>
#include <getopt.h>
#include <string>
#include <thread>
#include <vector>

static std::atomic<bool> g_running{true};

// Forward libnetconf2 print messages into our logger.
static void nc_print_clb(const struct nc_session * /*session*/,
                         NC_VERB_LEVEL level, const char *msg) {
  using logger::Level;
  auto &log = logger::get();
  switch (level) {
  case NC_VERB_ERROR:
    log.error(msg ? msg : "");
    break;
  case NC_VERB_WARNING:
    log.warn(msg ? msg : "");
    break;
  case NC_VERB_VERBOSE:
    log.info(msg ? msg : "");
    break;
  case NC_VERB_DEBUG:
  case NC_VERB_DEBUG_LOWLVL:
    log.debug(msg ? msg : "");
    break;
  default:
    log.info(msg ? msg : "");
    break;
  }
}

int main(int argc, char **argv) {
  auto &log = logger::get();
  log.info("netd: starting");

  // Simple CLI: allow configuring listening endpoints.
  struct Endpoint {
    enum class Type { Unix, SSH, TLS, Tcp } type;
    std::string addr;
    uint16_t port{};
    std::string extra; // for unix path or other params
  };

  std::vector<Endpoint> endpoints;

  // Default unix socket path when none is specified (netconf build only)
#ifdef STELLERI_NETCONF
  const std::string default_unix_socket = "/var/run/netconf.sock";
#endif

  const char *short_opts = "h";
  const struct option long_opts[] = {{"help", no_argument, nullptr, 'h'},
                                     {"unix", required_argument, nullptr, 0},
                                     {"tcp", required_argument, nullptr, 0},
                                     {"ssh", required_argument, nullptr, 0},
                                     {"tls", required_argument, nullptr, 0},
                                     {nullptr, 0, nullptr, 0}};

  int idx = 0;
  int c;
  while ((c = getopt_long(argc, argv, short_opts, long_opts, &idx)) != -1) {
    if (c == 'h') {
      std::puts("Usage: netd [--unix PATH] [--tcp ADDR:PORT] [--ssh ADDR:PORT] "
                "[--tls ADDR:PORT]");
      return 0;
    } else if (c == 0) {
      const char *name = long_opts[idx].name;
      if (std::strcmp(name, "unix") == 0) {
        Endpoint e;
        e.type = Endpoint::Type::Unix;
        e.extra = optarg;
        endpoints.push_back(std::move(e));
      } else if (std::strcmp(name, "tcp") == 0) {
        // format address:port or port
        Endpoint e;
        e.type = Endpoint::Type::Tcp;
        std::string arg(optarg);
        auto p = arg.find(':');
        if (p == std::string::npos) {
          e.addr = "0.0.0.0";
          e.port = static_cast<uint16_t>(std::stoi(arg));
        } else {
          e.addr = arg.substr(0, p);
          e.port = static_cast<uint16_t>(std::stoi(arg.substr(p + 1)));
        }
        endpoints.push_back(std::move(e));
      } else if (std::strcmp(name, "ssh") == 0) {
        Endpoint e;
        e.type = Endpoint::Type::SSH;
        std::string arg(optarg);
        auto p = arg.find(':');
        if (p == std::string::npos) {
          e.addr = "0.0.0.0";
          e.port = NC_PORT_SSH;
        } else {
          e.addr = arg.substr(0, p);
          e.port = static_cast<uint16_t>(std::stoi(arg.substr(p + 1)));
        }
        endpoints.push_back(std::move(e));
      } else if (std::strcmp(name, "tls") == 0) {
        Endpoint e;
        e.type = Endpoint::Type::TLS;
        std::string arg(optarg);
        auto p = arg.find(':');
        if (p == std::string::npos) {
          e.addr = "0.0.0.0";
          e.port = NC_PORT_TLS;
        } else {
          e.addr = arg.substr(0, p);
          e.port = static_cast<uint16_t>(std::stoi(arg.substr(p + 1)));
        }
        endpoints.push_back(std::move(e));
      }
    }
  }

  log.info("netd: initializing libnetconf2");

  // Route libnetconf2 internal prints to our logger.
  nc_set_print_clb_session(nc_print_clb);
  nc_verbosity(NC_VERB_WARNING);

  if (nc_server_init() != 0) {
    log.error("netd: nc_server_init() failed");
    return 1;
  }

  // Initialize or obtain a libyang context for the server.
  struct ly_ctx *ctx = nullptr;
  if (nc_server_init_ctx(&ctx) != 0) {
    log.warn("netd: nc_server_init_ctx() reported an issue (continuing)");
  }

  // If endpoints were requested on the CLI, build server configuration and
  // apply it.
  if (endpoints.empty()) {
#ifdef STELLERI_NETCONF
    Endpoint e;
    e.type = Endpoint::Type::Unix;
    e.extra = default_unix_socket;
    endpoints.push_back(std::move(e));
#endif
  }

  if (!endpoints.empty()) {
    if (nc_server_config_load_modules(&ctx) != 0) {
      log.error("netd: failed to load server config modules");
    } else {
      struct lyd_node *config = nullptr;
      unsigned count = 0;
      for (const auto &ep : endpoints) {
        std::string name = "ep" + std::to_string(count++);
        if (ep.type == Endpoint::Type::Unix) {
          if (nc_server_config_add_unix_socket(
                  ctx, name.c_str(), ep.extra.c_str(), nullptr, nullptr,
                  nullptr, &config) != 0) {
            log.warn("netd: failed to add unix socket endpoint");
          }
        } else {
#ifdef NC_ENABLED_SSH_TLS
          // For SSH/TLS/TCP use address/port. Map TCP->TLS/SSH transport where
          // applicable.
          NC_TRANSPORT_IMPL ti =
              (ep.type == Endpoint::Type::SSH) ? NC_TI_SSH : NC_TI_TLS;
          if (ep.type == Endpoint::Type::Tcp)
            ti = NC_TI_TLS; // TCP treated as TLS by default
          if (nc_server_config_add_address_port(ctx, name.c_str(), ti,
                                                ep.addr.c_str(), ep.port,
                                                &config) != 0) {
            log.warn("netd: failed to add address/port endpoint");
          }
#else
          log.warn("netd: SSH/TLS endpoints not supported by libnetconf2 "
                   "build; skipping");
#endif
        }
      }

      if (config) {
        if (nc_server_config_setup_data(config) != 0) {
          log.warn("netd: applying server configuration failed");
        }
      }
    }
  }

  Server srv;
  if (!srv.registerCallbacks()) {
    log.warn("netd: Server::registerCallbacks() failed (continuing)");
  }

  // Install signal handlers
  std::signal(SIGINT, [](int) { g_running.store(false); });
  std::signal(SIGTERM, [](int) { g_running.store(false); });

  // Main loop
  while (g_running.load()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  log.info("netd: shutting down");
  srv.unregisterCallbacks();
  nc_server_destroy();
  return 0;
}
