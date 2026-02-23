/*
 * Client.hpp
 * Minimal libnetconf2 client-side wrapper.
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfServerReply.hpp"
#include "Session.hpp"
#include "YangContext.hpp"
#include "YangData.hpp"
#include "NetconfEditConfigOperation.hpp"
#include "DataStore.hpp"
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

  // RPC helpers: convenience wrappers that build and send NETCONF RPCs
  // using the configured session. Return a server-style reply wrapper.
  std::unique_ptr<NetconfServerReply> get(const YangData &filter) const;
  std::unique_ptr<NetconfServerReply> getConfig(const YangData &filter) const;
  std::unique_ptr<NetconfServerReply>
  editConfig(const YangData &target, NetconfEditConfigOperation op) const;
  std::unique_ptr<NetconfServerReply> copyConfig(DataStore src,
                                                 DataStore dst) const;
  std::unique_ptr<NetconfServerReply>
  deleteConfig(const YangData &target) const;
  std::unique_ptr<NetconfServerReply> commit() const;
  std::unique_ptr<NetconfServerReply> lock(DataStore ds) const;
  std::unique_ptr<NetconfServerReply> unlock(DataStore ds) const;
  std::unique_ptr<NetconfServerReply> closeSession() const;
  std::unique_ptr<NetconfServerReply> killSession(const Session &target) const;
  std::unique_ptr<NetconfServerReply> validate(const YangData &target) const;
  std::unique_ptr<NetconfServerReply> discardChanges() const;

private:
  Session session_;
};
