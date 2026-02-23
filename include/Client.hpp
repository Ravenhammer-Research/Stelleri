/*
 * Client.hpp
 * Minimal libnetconf2 client-side wrapper.
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Session.hpp"
#include "YangContext.hpp"
#include <memory>
#include <string>

class Client {
public:
  explicit Client(Session session) noexcept;
  ~Client();

  Client(const Client &) = delete;
  Client &operator=(const Client &) = delete;

  Client(Client &&) noexcept = delete;
  Client &operator=(Client &&) noexcept = delete;

  // Initialize the global singleton client from a unix socket path.
  // Returns true on success and makes the client available via `instance()`.
  static bool init_unix(const std::string &path,
                        const YangContext &ctx = YangContext());

  // Initialize the global singleton client from existing file descriptors.
  static bool init_inout(int fdin, int fdout,
                         const YangContext &ctx = YangContext());

#ifdef NC_ENABLED_SSH_TLS
  static bool init_ssh(const std::string &host, uint16_t port,
                       const YangContext &ctx = YangContext());
  static bool init_tls(const std::string &host, uint16_t port,
                       const YangContext &ctx = YangContext());
#endif

  // Access the configured singleton client; returns nullptr if not initialized.
  static Client *instance() noexcept;

  // Shutdown and free the singleton client.
  static void shutdown() noexcept;

  // Lower-level factories (return unique_ptr) retained for internal/testing
  // use.
  static std::unique_ptr<Client>
  connect_unix(const std::string &path, const YangContext &ctx = YangContext());
  static std::unique_ptr<Client>
  connect_inout(int fdin, int fdout, const YangContext &ctx = YangContext());
#ifdef NC_ENABLED_SSH_TLS
  static std::unique_ptr<Client>
  connect_ssh(const std::string &host, uint16_t port,
              const YangContext &ctx = YangContext());
  static std::unique_ptr<Client>
  connect_tls(const std::string &host, uint16_t port,
              const YangContext &ctx = YangContext());
#endif

  const Session &session() const noexcept { return session_; }

private:
  Session session_;
};
