/*
 * Client.cpp
 * Minimal libnetconf2 client-side wrapper implementation.
 */

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf sources are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"

#include <libnetconf2/session.h>
#include <libnetconf2/session_client.h>
#include <cstdint>

// libnetconf2 headers may include libyang headers that trigger -Werror on
// anonymous/gnu extensions; silence those diagnostics locally.
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <libnetconf2/session_client.h>
#include <libnetconf2/session.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <cstdlib>
#include <mutex>
#ifdef NC_ENABLED_SSH_TLS
#include <libnetconf2/session_client.h>
#endif

Client::Client(Session session) noexcept : session_(std::move(session)) {}

Client::~Client() = default;

std::unique_ptr<Client> Client::connect_unix(const std::string &path,
                                             const YangContext &ctx) {
  struct nc_session *s = nc_connect_unix(path.c_str(), ctx.get());
  if (!s)
    return nullptr;
  Session sess(s);
  return std::unique_ptr<Client>(new Client(std::move(sess)));
}

std::unique_ptr<Client> Client::connect_inout(int fdin, int fdout,
                                              const YangContext &ctx) {
  struct nc_session *s = nc_connect_inout(fdin, fdout, ctx.get());
  if (!s)
    return nullptr;
  Session sess(s);
  return std::unique_ptr<Client>(new Client(std::move(sess)));
}

#ifdef NC_ENABLED_SSH_TLS
std::unique_ptr<Client> Client::connect_ssh(const std::string &host,
                                            uint16_t port,
                                            const YangContext &ctx) {
  struct nc_session *s = nc_connect_ssh(host.c_str(), port, ctx.get());
  if (!s)
    return nullptr;
  Session sess(s);
  return std::unique_ptr<Client>(new Client(std::move(sess)));
}

std::unique_ptr<Client> Client::connect_tls(const std::string &host,
                                            uint16_t port,
                                            const YangContext &ctx) {
  struct nc_session *s = nc_connect_tls(host.c_str(), port, ctx.get());
  if (!s)
    return nullptr;
  Session sess(s);
  return std::unique_ptr<Client>(new Client(std::move(sess)));
}
#endif

// Singleton storage
static std::unique_ptr<Client> g_client_instance;
static std::mutex g_client_mutex;

bool Client::init_unix(const std::string &path, const YangContext &ctx) {
  auto tmp = Client::connect_unix(path, ctx);
  if (!tmp)
    return false;
  std::lock_guard<std::mutex> lk(g_client_mutex);
  g_client_instance = std::move(tmp);
  return true;
}

bool Client::init_inout(int fdin, int fdout, const YangContext &ctx) {
  auto tmp = Client::connect_inout(fdin, fdout, ctx);
  if (!tmp)
    return false;
  std::lock_guard<std::mutex> lk(g_client_mutex);
  g_client_instance = std::move(tmp);
  return true;
}

#ifdef NC_ENABLED_SSH_TLS
bool Client::init_ssh(const std::string &host, uint16_t port,
                      const YangContext &ctx) {
  auto tmp = Client::connect_ssh(host, port, ctx);
  if (!tmp)
    return false;
  std::lock_guard<std::mutex> lk(g_client_mutex);
  g_client_instance = std::move(tmp);
  return true;
}

bool Client::init_tls(const std::string &host, uint16_t port,
                      const YangContext &ctx) {
  auto tmp = Client::connect_tls(host, port, ctx);
  if (!tmp)
    return false;
  std::lock_guard<std::mutex> lk(g_client_mutex);
  g_client_instance = std::move(tmp);
  return true;
}
#endif

Client *Client::instance() noexcept {
  std::lock_guard<std::mutex> lk(g_client_mutex);
  return g_client_instance.get();
}

void Client::shutdown() noexcept {
  std::lock_guard<std::mutex> lk(g_client_mutex);
  g_client_instance.reset();
}
