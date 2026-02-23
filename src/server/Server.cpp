/*
 * Server.cpp
 * Minimal stub implementations for `Server` declared in Server.hpp.
 * The C callback returns a default OK reply; no parsing or helpers.
 */

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Server.hpp"
#include "DataStore.hpp"
#include "NetconfEditConfigOperation.hpp"
#include "NetconfExecutor.hpp"
#include "NetconfServerReply.hpp"
#include "Session.hpp"
#include "YangData.hpp"

#include <cstring>
#include <memory>

#include <libnetconf2/messages_server.h>
#include <libyang/libyang.h>

Server::Server() noexcept {}

Server::~Server() = default;

bool Server::registerCallbacks() { return true; }

void Server::unregisterCallbacks() { /* no-op for now */ }

struct nc_server_reply *
Server::server_rpc_callback(struct nc_session *session,
                            const struct lyd_node *rpc,
                            void * /*private_data*/) noexcept {
  Session s(session);
  std::unique_ptr<NetconfServerReply> reply;

  // Determine RPC operation name from the first child element of <rpc>.
  const struct lyd_node *op = rpc ? lyd_child(rpc) : nullptr;
  const char *opname = nullptr;
  if (op) {
    const struct lysc_node *schema = op->schema;
    if (schema && schema->name)
      opname = schema->name;
  }

  // Extract the operation payload (duplicate so YangData owns it).
  struct lyd_node *data_node = nullptr;
  if (op) {
    const struct lyd_node *payload = lyd_child(op);
    if (payload) {
      (void)lyd_dup_single(payload, nullptr, 0, &data_node);
    } else {
      (void)lyd_dup_single(op, nullptr, 0, &data_node);
    }
  }
  // Construct YangData from the duplicated payload node.
  YangData data(data_node);

  if (!opname) {
    // No operation - return OK by default.
    return nc_server_reply_ok();
  }

  if (std::strcmp(opname, "get") == 0) {
    reply = NetconfExecutor::get(s, data);
  } else if (std::strcmp(opname, "get-config") == 0) {
    reply = NetconfExecutor::getConfig(s, data);
  } else if (std::strcmp(opname, "edit-config") == 0) {
    reply =
        NetconfExecutor::editConfig(s, data, NetconfEditConfigOperation::Merge);
  } else if (std::strcmp(opname, "copy-config") == 0) {
    reply =
        NetconfExecutor::copyConfig(s, DataStore::Running, DataStore::Running);
  } else if (std::strcmp(opname, "delete-config") == 0) {
    reply = NetconfExecutor::deleteConfig(s, data);
  } else if (std::strcmp(opname, "commit") == 0) {
    reply = NetconfExecutor::commit(s);
  } else if (std::strcmp(opname, "lock") == 0) {
    reply = NetconfExecutor::lock(s, DataStore::Running);
  } else if (std::strcmp(opname, "unlock") == 0) {
    reply = NetconfExecutor::unlock(s, DataStore::Running);
  } else if (std::strcmp(opname, "close-session") == 0) {
    reply = NetconfExecutor::closeSession(s, Session());
  } else if (std::strcmp(opname, "kill-session") == 0) {
    reply = NetconfExecutor::killSession(s, Session());
  } else if (std::strcmp(opname, "validate") == 0) {
    reply = NetconfExecutor::validate(s, data);
  } else if (std::strcmp(opname, "discard-changes") == 0) {
    reply = NetconfExecutor::discardChanges(s);
  } else {
    // Unknown operation: return OK as a conservative default.
    return nc_server_reply_ok();
  }

  if (!reply) {
    struct lyd_node *err = nc_err(nullptr, NC_ERR_OP_FAILED);
    return nc_server_reply_err(err);
  }

  return reply->toNcServerReply();
}
