/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfExecutor.hpp"
#include "ArpConfig.hpp"
#include "DataStore.hpp"
#include "InterfaceConfig.hpp"
#include "NdpConfig.hpp"
#include "NetconfEditConfigOperation.hpp"
#include "NetconfServerReply.hpp"
#include "PolicyConfig.hpp"
#include "RouteConfig.hpp"
#include "Session.hpp"
#include "SystemConfigurationManager.hpp"
#include "VRFConfig.hpp"
#include "YangContext.hpp"
#include "YangData.hpp"
#include <cstring>
#include <libyang/libyang.h>
#include <memory>
#include <mutex>

#include "IetfInterfaces.hpp"
#include "Logger.hpp"

namespace {

  // Operation to apply on commit
  struct CommitOperation {
    enum Category { Interface, Route, Vrf, Policy, Arp, Ndp };
    enum Op { Create, Merge, Delete, Set };

    Category category;
    Op operation;

    // Interface fields
    std::string interfaceName;
    std::unique_ptr<InterfaceConfig> ifaceConfig;

    // Route fields
    std::unique_ptr<RouteConfig> routeConfig;

    // VRF fields
    std::unique_ptr<VRFConfig> vrfConfig;

    // Policy fields
    std::unique_ptr<PolicyConfig> policyConfig;

    // ARP/NDP fields
    std::string ipAddr;
    std::string macAddr;
    std::optional<std::string> ifaceFilter;
    bool temp = false;
    bool pub = false;
  };

  std::mutex g_ds_mutex;
  struct lyd_node *g_running = nullptr;
  struct lyd_node *g_candidate = nullptr;

  // Helper to determine the category of a YANG node
  static CommitOperation::Category getCategory(const struct lyd_node *node) {
    if (!node || !node->schema || !node->schema->module)
      return CommitOperation::Interface;

    const char *moduleName = node->schema->module->name;
    const char *nodeName = node->schema->name;

    // Check module name first
    if (std::strcmp(moduleName, "ietf-routing") == 0 ||
        std::strcmp(moduleName, "ietf-network-instance") == 0) {
      return CommitOperation::Route;
    }
    if (std::strcmp(moduleName, "stelleri-access-control") == 0) {
      return CommitOperation::Policy;
    }
    if (std::strcmp(moduleName, "stelleri-arp") == 0) {
      return CommitOperation::Arp;
    }
    if (std::strcmp(moduleName, "stelleri-ndp") == 0) {
      return CommitOperation::Ndp;
    }

    // Check node name
    if (std::strcmp(nodeName, "interface") == 0)
      return CommitOperation::Interface;
    if (std::strcmp(nodeName, "access-list") == 0 ||
        std::strcmp(nodeName, "policy") == 0)
      return CommitOperation::Policy;
    if (std::strcmp(nodeName, "entry") == 0 &&
        std::strstr(moduleName ? moduleName : "", "arp"))
      return CommitOperation::Arp;
    if (std::strcmp(nodeName, "entry") == 0 &&
        std::strstr(moduleName ? moduleName : "", "ndp"))
      return CommitOperation::Ndp;

    return CommitOperation::Interface;
  }

  // Helper to extract edit operation from a node's metadata
  static NetconfEditConfigOperation
  getEditOperation(const struct lyd_node *node) {
    if (node && node->metadata) {
      for (struct lyd_meta *m = node->metadata; m; m = m->next) {
        if (m->name && std::strcmp(m->name, "operation") == 0) {
          const char *val = lyd_get_meta_value(m);
          if (val) {
            if (std::strcmp(val, "merge") == 0)
              return NetconfEditConfigOperation::Merge;
            if (std::strcmp(val, "replace") == 0)
              return NetconfEditConfigOperation::Replace;
            if (std::strcmp(val, "create") == 0)
              return NetconfEditConfigOperation::Create;
            if (std::strcmp(val, "delete") == 0)
              return NetconfEditConfigOperation::Delete;
            if (std::strcmp(val, "remove") == 0)
              return NetconfEditConfigOperation::Remove;
          }
        }
      }
    }
    return NetconfEditConfigOperation::Merge;
  }

  // Helper to extract interface name from a YANG node
  static std::string getInterfaceName(const struct lyd_node *node) {
    if (!node)
      return "";

    struct lyd_node *nameNode = nullptr;
    if (lyd_find_path(node, "name", 0, &nameNode) == LY_SUCCESS && nameNode) {
      const char *val = lyd_get_value(nameNode);
      if (val)
        return std::string(val);
    }
    return "";
  }

  // Helper to extract route prefix from a YANG node
  static std::string getRoutePrefix(const struct lyd_node *node) {
    if (!node)
      return "";

    struct lyd_node *prefixNode = nullptr;
    if (lyd_find_path(node, "destination-prefix", 0, &prefixNode) ==
            LY_SUCCESS &&
        prefixNode) {
      const char *val = lyd_get_value(prefixNode);
      if (val)
        return std::string(val);
    }
    return "";
  }

  // Helper to extract VRF name from a YANG node
  static std::string getVrfName(const struct lyd_node *node) {
    if (!node)
      return "";

    struct lyd_node *nameNode = nullptr;
    if (lyd_find_path(node, "name", 0, &nameNode) == LY_SUCCESS && nameNode) {
      const char *val = lyd_get_value(nameNode);
      if (val)
        return std::string(val);
    }
    return "";
  }

  // Helper to extract ARP IP from a YANG node
  static std::string getArpIp(const struct lyd_node *node) {
    if (!node)
      return "";

    struct lyd_node *ipNode = nullptr;
    if (lyd_find_path(node, "ip", 0, &ipNode) == LY_SUCCESS && ipNode) {
      const char *val = lyd_get_value(ipNode);
      if (val)
        return std::string(val);
    }
    return "";
  }

  // Helper to extract NDP IP from a YANG node
  static std::string getNdpIp(const struct lyd_node *node) {
    if (!node)
      return "";

    struct lyd_node *ipNode = nullptr;
    if (lyd_find_path(node, "ipv6-address", 0, &ipNode) == LY_SUCCESS &&
        ipNode) {
      const char *val = lyd_get_value(ipNode);
      if (val)
        return std::string(val);
    }
    if (lyd_find_path(node, "ip", 0, &ipNode) == LY_SUCCESS && ipNode) {
      const char *val = lyd_get_value(ipNode);
      if (val)
        return std::string(val);
    }
    return "";
  }

  // Helper to extract access-list ID from a YANG node
  static uint32_t getAccessListId(const struct lyd_node *node) {
    if (!node)
      return 0;

    struct lyd_node *idNode = nullptr;
    if (lyd_find_path(node, "name", 0, &idNode) == LY_SUCCESS && idNode) {
      const char *val = lyd_get_value(idNode);
      if (val)
        return std::strtoul(val, nullptr, 10);
    }
    return 0;
  }

} // namespace

void NetconfExecutor::init(const struct ly_ctx *ctx) {
  auto &log = logger::get();
  std::lock_guard<std::mutex> lock(g_ds_mutex);
  if (g_running)
    return;

  log.debug("NetconfExecutor: initializing running datastore");

  // Create the root node for ietf-interfaces. Use lyd_new_path to ensure it's
  // properly initialized in the context.
  if (lyd_new_path(nullptr, ctx, "/ietf-interfaces:interfaces", nullptr, 0,
                   &g_running) != LY_SUCCESS) {
    log.warn("NetconfExecutor: failed to create /ietf-interfaces:interfaces "
             "via lyd_new_path");
    // If ietf-interfaces is not found or fails, try to just get the module and
    // use it.
    const struct lys_module *mod =
        ly_ctx_get_module_implemented(ctx, "ietf-interfaces");
    if (mod) {
      log.debug(
          "NetconfExecutor: found ietf-interfaces module, using lyd_new_inner");
      lyd_new_inner(nullptr, mod, "interfaces", 0, &g_running);
    } else {
      log.error("NetconfExecutor: ietf-interfaces module not implemented in "
                "context!");
    }
  }

  if (g_running) {
    log.debug("NetconfExecutor: running datastore initialized successfully");
  } else {
    log.error("NetconfExecutor: failed to initialize running datastore");
  }
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::get(const Session &session, const YangData &filter) {
  return getConfig(session, filter);
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::getConfig(const Session &session,
                           const YangData &filter [[maybe_unused]]) {
  auto &log = logger::get();
  log.debug("NetconfExecutor: getConfig called");
  std::lock_guard<std::mutex> lock(g_ds_mutex);
  struct lyd_node *dup = nullptr;

  if (g_running) {
    // Use LYD_DUP_RECURSIVE (0x01) to clone the entire tree.
    if (lyd_dup_siblings(g_running, nullptr, 0x01, &dup) != LY_SUCCESS) {
      log.error("NetconfExecutor: failed to duplicate running datastore");
      auto r =
          std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
      r->setError(NetconfServerReply::ERR_OP_FAILED,
                  session.yangContext().get());
      return r;
    }
    log.debug("NetconfExecutor: duplicated running datastore for reply");
  } else {
    log.warn("NetconfExecutor: g_running is NULL in getConfig");
  }

  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_DATA);
  r->setData(YangData(dup));
  return r;
}

// Commit operations queue - populated during edit-config, executed on commit
static std::vector<CommitOperation> g_commitQueue;

std::unique_ptr<NetconfServerReply>
NetconfExecutor::editConfig(const Session &session, const YangData &target,
                            NetconfEditConfigOperation op) {
  auto &log = logger::get();
  std::lock_guard<std::mutex> lock(g_ds_mutex);

  // Find the actual config nodes. If 'target' is the 'config' node of
  // edit-config, we need its children.
  const struct lyd_node *src = target.toLydNode();
  if (!src) {
    return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
  }

  const struct lyd_node *config_node = src;
  if (src->schema && src->schema->name &&
      std::strcmp(src->schema->name, "config") == 0) {
    config_node = lyd_child(src);
  }

  if (!config_node) {
    return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
  }

  // Check the root node's operation attribute first (highest priority)
  NetconfEditConfigOperation effective_op = getEditOperation(config_node);

  // If op was explicitly passed as something other than Merge, use it
  if (op != NetconfEditConfigOperation::Merge) {
    effective_op = op;
    log.debug(
        std::format("NetconfExecutor: editConfig using explicit operation: {}",
                    static_cast<int>(effective_op)));
  } else if (effective_op != NetconfEditConfigOperation::Merge) {
    log.debug(std::format(
        "NetconfExecutor: editConfig detected operation from node metadata: {}",
        static_cast<int>(effective_op)));
  }

  // Initialize candidate with a clone of g_running if it's empty.
  if (!g_candidate) {
    if (g_running) {
      lyd_dup_siblings(g_running, nullptr, 0x01, &g_candidate);
    } else {
      const struct lys_module *mod = ly_ctx_get_module_implemented(
          session.yangContext().get(), "ietf-interfaces");
      if (mod) {
        lyd_new_inner(nullptr, mod, "interfaces", 0, &g_candidate);
      }
    }
  }

  // Parse all configurations from the input, categorizing by node type
  std::vector<std::pair<struct lyd_node *, NetconfEditConfigOperation>>
      opsToQueue;

  const struct lyd_node *node = config_node;
  while (node) {
    CommitOperation::Category cat = getCategory(node);
    NetconfEditConfigOperation nodeOp = getEditOperation(node);

    // If parent has a different effective_op, prefer it for this iteration
    if (effective_op != NetconfEditConfigOperation::Merge &&
        nodeOp == NetconfEditConfigOperation::Merge) {
      nodeOp = effective_op;
    }

    opsToQueue.emplace_back(const_cast<struct lyd_node *>(node), nodeOp);

    node = node->next;
  }

  // Store operations in the commit queue
  for (const auto &[node, operation] : opsToQueue) {
    CommitOperation cmd;
    cmd.category = getCategory(node);
    cmd.operation = (operation == NetconfEditConfigOperation::Create)
                        ? CommitOperation::Op::Create
                        : ((operation == NetconfEditConfigOperation::Delete ||
                            operation == NetconfEditConfigOperation::Remove)
                               ? CommitOperation::Op::Delete
                               : CommitOperation::Op::Merge);

    switch (cmd.category) {
    case CommitOperation::Interface: {
      cmd.interfaceName = getInterfaceName(node);
      if (!cmd.interfaceName.empty()) {
        IetfInterfaces ii(const_cast<struct lyd_node *>(node));
        cmd.ifaceConfig =
            std::make_unique<InterfaceConfig>(ii.toInterfaceConfig());
        g_commitQueue.push_back(std::move(cmd));
        log.debug(std::format(
            "NetconfExecutor: queued {} for interface {}",
            (cmd.operation == CommitOperation::Op::Create ? "create" : "merge"),
            cmd.interfaceName));
      }
      break;
    }

    case CommitOperation::Route: {
      std::string prefix = getRoutePrefix(node);
      if (!prefix.empty()) {
        cmd.routeConfig =
            std::make_unique<RouteConfig>(RouteConfig::fromYang(node));
        g_commitQueue.push_back(std::move(cmd));
        log.debug(std::format(
            "NetconfExecutor: queued {} for route {}",
            (cmd.operation == CommitOperation::Op::Create ? "create" : "merge"),
            prefix));
      }
      break;
    }

    case CommitOperation::Vrf: {
      std::string vrfName = getVrfName(node);
      if (!vrfName.empty()) {
        cmd.vrfConfig = std::make_unique<VRFConfig>(VRFConfig::fromYang(node));
        g_commitQueue.push_back(std::move(cmd));
        log.debug(std::format(
            "NetconfExecutor: queued {} for VRF {}",
            (cmd.operation == CommitOperation::Op::Create ? "create" : "merge"),
            vrfName));
      }
      break;
    }

    case CommitOperation::Policy: {
      uint32_t aclId = getAccessListId(node);
      if (aclId > 0) {
        cmd.policyConfig =
            std::make_unique<PolicyConfig>(PolicyConfig::fromYang(node));
        g_commitQueue.push_back(std::move(cmd));
        log.debug(std::format(
            "NetconfExecutor: queued {} for policy {}",
            (cmd.operation == CommitOperation::Op::Create ? "create" : "merge"),
            aclId));
      }
      break;
    }

    case CommitOperation::Arp: {
      std::string ip = getArpIp(node);
      if (!ip.empty()) {
        cmd.ipAddr = ip;
        // TODO: Store ArpConfig directly instead of via InterfaceConfig
        auto arp = ArpConfig::fromYang(node);
        g_commitQueue.push_back(std::move(cmd));
        log.debug(std::format(
            "NetconfExecutor: queued {} for ARP {}",
            (cmd.operation == CommitOperation::Op::Create ? "create" : "merge"),
            ip));
      }
      break;
    }

    case CommitOperation::Ndp: {
      std::string ip = getNdpIp(node);
      if (!ip.empty()) {
        cmd.ipAddr = ip;
        auto ndp = NdpConfig::fromYang(node);
        g_commitQueue.push_back(std::move(cmd));
        log.debug(std::format(
            "NetconfExecutor: queued {} for NDP {}",
            (cmd.operation == CommitOperation::Op::Create ? "create" : "merge"),
            ip));
      }
      break;
    }
    }
  }

  // Merge into candidate datastore (for non-delete operations)
  if (effective_op == NetconfEditConfigOperation::Merge ||
      effective_op == NetconfEditConfigOperation::Replace) {
    if (lyd_merge_siblings(&g_candidate, config_node, 0) != LY_SUCCESS) {
      auto r =
          std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
      r->setError(NetconfServerReply::ERR_OP_FAILED,
                  session.yangContext().get());
      return r;
    }
  }

  // For delete/remove, also remove from candidate
  if (effective_op == NetconfEditConfigOperation::Delete ||
      effective_op == NetconfEditConfigOperation::Remove) {
    for (const auto &[node, _] : opsToQueue) {
      lyd_free_tree(const_cast<struct lyd_node *>(node));
    }
  }

  return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::commit(const Session &session) {
  auto &log = logger::get();
  std::lock_guard<std::mutex> lock(g_ds_mutex);

  if (g_commitQueue.empty()) {
    log.debug("NetconfExecutor: commit called with no pending operations");
    return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
  }

  SystemConfigurationManager scm;
  bool success = true;

  // Execute each operation in the queue
  for (auto &cmd : g_commitQueue) {
    try {
      switch (cmd.category) {
      case CommitOperation::Interface: {
        if (cmd.operation == CommitOperation::Op::Create) {
          log.info(std::format("NetconfExecutor: creating interface {}",
                               cmd.interfaceName));

          // Check if interface already exists
          if (scm.InterfaceExists(cmd.interfaceName)) {
            auto r = std::make_unique<NetconfServerReply>(
                NetconfServerReply::RPL_ERROR);
            r->setError(NetconfServerReply::ERR_DATA_EXISTS,
                        session.yangContext().get());
            g_commitQueue.clear();
            return r;
          }

          scm.CreateInterface(cmd.interfaceName);

          if (cmd.ifaceConfig) {
            scm.SaveInterface(*cmd.ifaceConfig);
          }
        } else if (cmd.operation == CommitOperation::Op::Merge) {
          log.info(
              std::format("NetconfExecutor: merging config for interface {}",
                          cmd.interfaceName));

          if (!scm.InterfaceExists(cmd.interfaceName)) {
            scm.CreateInterface(cmd.interfaceName);
          }

          if (cmd.ifaceConfig) {
            scm.SaveInterface(*cmd.ifaceConfig);
          }
        } else if (cmd.operation == CommitOperation::Op::Delete) {
          log.info(std::format("NetconfExecutor: deleting interface {}",
                               cmd.interfaceName));

          if (scm.InterfaceExists(cmd.interfaceName)) {
            scm.DestroyInterface(cmd.interfaceName);
          }
        }
        break;
      }

      case CommitOperation::Route: {
        // TODO: Implement route operations
        log.warn(std::format(
            "NetconfExecutor: route {} not yet implemented",
            (cmd.operation == CommitOperation::Op::Create   ? "create"
             : cmd.operation == CommitOperation::Op::Delete ? "delete"
                                                            : "merge")));
        break;
      }

      case CommitOperation::Vrf: {
        // TODO: Implement VRF operations
        log.warn(std::format(
            "NetconfExecutor: VRF {} not yet implemented",
            (cmd.operation == CommitOperation::Op::Create   ? "create"
             : cmd.operation == CommitOperation::Op::Delete ? "delete"
                                                            : "merge")));
        break;
      }

      case CommitOperation::Policy: {
        // TODO: Implement policy operations
        log.warn(std::format(
            "NetconfExecutor: policy {} not yet implemented",
            (cmd.operation == CommitOperation::Op::Create   ? "create"
             : cmd.operation == CommitOperation::Op::Delete ? "delete"
                                                            : "merge")));
        break;
      }

      case CommitOperation::Arp: {
        // TODO: Implement ARP operations
        log.warn(std::format(
            "NetconfExecutor: ARP {} not yet implemented for {}",
            (cmd.operation == CommitOperation::Op::Create   ? "create"
             : cmd.operation == CommitOperation::Op::Delete ? "delete"
                                                            : "merge"),
            cmd.ipAddr));
        break;
      }

      case CommitOperation::Ndp: {
        // TODO: Implement NDP operations
        log.warn(std::format(
            "NetconfExecutor: NDP {} not yet implemented for {}",
            (cmd.operation == CommitOperation::Op::Create   ? "create"
             : cmd.operation == CommitOperation::Op::Delete ? "delete"
                                                            : "merge"),
            cmd.ipAddr));
        break;
      }
      }
    } catch (const std::exception &e) {
      log.error(std::format("NetconfExecutor: failed to apply operation: {}",
                            e.what()));
      success = false;
      break;
    }
  }

  // Clear the commit queue after processing
  g_commitQueue.clear();

  if (!success) {
    auto r =
        std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
    r->setError(NetconfServerReply::ERR_OP_FAILED, session.yangContext().get());
    return r;
  }

  // Update running datastore to match current candidate state
  lyd_free_all(g_running);
  g_running = nullptr;

  if (g_candidate) {
    lyd_dup_siblings(g_candidate, nullptr, 0x01, &g_running);
  }

  return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::copyConfig(const Session &session [[maybe_unused]],
                            DataStore src, DataStore dst) {
  std::lock_guard<std::mutex> lock(g_ds_mutex);

  struct lyd_node **src_ptr =
      (src == DataStore::Running) ? &g_running : &g_candidate;
  struct lyd_node **dst_ptr =
      (dst == DataStore::Running) ? &g_running : &g_candidate;

  if (*dst_ptr) {
    lyd_free_all(*dst_ptr);
    *dst_ptr = nullptr;
  }

  if (*src_ptr) {
    lyd_dup_siblings(*src_ptr, nullptr, 0x01, dst_ptr);
  }

  return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::deleteConfig(const Session &session [[maybe_unused]],
                              const YangData &target [[maybe_unused]]) {
  std::lock_guard<std::mutex> lock(g_ds_mutex);
  if (g_candidate) {
    lyd_free_all(g_candidate);
    g_candidate = nullptr;
  }
  return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
}

std::unique_ptr<NetconfServerReply> NetconfExecutor::lock(const Session &session
                                                          [[maybe_unused]],
                                                          DataStore ds
                                                          [[maybe_unused]]) {
  return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::unlock(const Session &session [[maybe_unused]],
                        DataStore ds [[maybe_unused]]) {
  return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::closeSession(const Session &session [[maybe_unused]],
                              const Session &target [[maybe_unused]]) {
  return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::killSession(const Session &session [[maybe_unused]],
                             const Session &target [[maybe_unused]]) {
  return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::validate(const Session &session [[maybe_unused]],
                          const YangData &target [[maybe_unused]]) {
  return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::discardChanges(const Session &session [[maybe_unused]]) {
  std::lock_guard<std::mutex> lock(g_ds_mutex);
  if (g_candidate) {
    lyd_free_all(g_candidate);
    g_candidate = nullptr;
  }
  // Also clear pending commit operations
  g_commitQueue.clear();
  return std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_OK);
}
