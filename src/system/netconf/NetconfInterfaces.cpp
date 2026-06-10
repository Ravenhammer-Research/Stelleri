#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "IetfInterfaces.hpp"
#include "NetconfConfigurationManager.hpp"
#include "NetconfError.hpp"
#include "YangContext.hpp"
#include "YangData.hpp"
#include "YangError.hpp"
#include <iostream>
#include <libyang/libyang.h>

std::vector<InterfaceConfig> NetconfConfigurationManager::GetInterfaces(
    const std::optional<VRFConfig> & /*vrf*/) const {
  // Use the global Client singleton to request the interfaces subtree.
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  YangContext yctx = c->session().yangContext();
  if (!yctx.get())
    throw YangError(yctx);

  YangModule ym = yctx.GetModule("ietf-interfaces");
  const struct lys_module *mod = ym.getModulePtr();

  if (!mod || !ym.valid()) {
    // Attempt to load it if it's missing from the context.
    mod = ly_ctx_load_module(const_cast<struct ly_ctx *>(yctx.get()),
                             "ietf-interfaces", nullptr, nullptr);
    if (!mod) {
      throw std::runtime_error("ietf-interfaces module not found in client "
                               "context and failed to load");
    }
    // Implement it so we can create data nodes.
    if (lys_set_implemented(const_cast<struct lys_module *>(mod), nullptr) !=
        LY_SUCCESS) {
      throw YangError(yctx);
    }
  }

  struct lyd_node *filter_node = nullptr;
  if (lyd_new_inner(nullptr, mod, "interfaces", 0, &filter_node) !=
      LY_SUCCESS) {
    throw YangError(yctx);
  }

  // Wrap the created filter node in a YangData so its destructor
  // will free the libyang data tree when it goes out of scope.
  YangData filter(filter_node, SubTree);

  // Use the Client helper to perform the get-config with the typed
  // subtree filter (avoid converting to XML and manually sending RPCs).
  std::unique_ptr<NetconfServerReply> reply = c->getConfig(filter);
  std::vector<InterfaceConfig> out;
  if (reply && reply->hasData()) {
    auto data = reply->getData();
    struct lyd_node *op = data ? data->toLydNode() : nullptr;
    if (op) {
      struct ly_set *set = nullptr;
      if (lyd_find_xpath(op, "/ietf-interfaces:interfaces/interface", &set) ==
              LY_SUCCESS &&
          set) {
        for (uint32_t i = 0; i < set->count; ++i) {
          struct lyd_node *n = set->dnodes[i];
          if (!n)
            continue;
          IetfInterfaces ii(n);
          out.push_back(ii.toInterfaceConfig());
        }
        ly_set_free(set, nullptr);
      }
    }
  }

  return out;
}

std::vector<InterfaceConfig> NetconfConfigurationManager::GetInterfacesByGroup(
    const std::optional<VRFConfig> & /*vrf*/,
    std::string_view /*group*/) const {
  return {};
}

void NetconfConfigurationManager::CreateInterface(
    const std::string &name) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Create an interface config with operation="create"
  YangContext yctx = c->session().yangContext();
  if (!yctx.get())
    throw YangError(yctx);

  const struct lys_module *mod =
      ly_ctx_get_module_implemented(yctx.get(), "ietf-interfaces");
  if (!mod)
    mod = ly_ctx_load_module(const_cast<struct ly_ctx *>(yctx.get()),
                             "ietf-interfaces", nullptr, nullptr);
  if (!mod)
    throw std::runtime_error("ietf-interfaces module not found");

  // Create the interface node with operation="create" metadata
  struct lyd_node *iface = nullptr;
  struct lyd_node *node = nullptr;

  // Create interface container
  lyd_new_inner(nullptr, mod, "interfaces", 0, &iface);

  // Create interface entry with name
  std::string iface_path =
      std::format("/ietf-interfaces:interfaces/interface[name='{}']", name);
  if (lyd_new_path(iface, yctx.get(), iface_path.c_str(), nullptr, 0, &node) !=
      LY_SUCCESS)
    throw YangError(yctx);

  // Add operation="create" metadata
  struct lyd_meta *meta = nullptr;
  if (lyd_new_meta(nullptr, node, "ietf-netconf:operation", "create", false,
                   &meta) != LY_SUCCESS)
    throw YangError(yctx);

  // Wrap in YangData and send via edit-config
  YangData data(iface);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Create);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr, yctx);
}

void NetconfConfigurationManager::SaveInterface(
    const InterfaceConfig &ic) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Convert to IetfInterfaces and then to lyd_node
  IetfInterfaces ii(ic);
  struct lyd_node *node = ii.toLydNode(c->session().yangContext());
  if (!node)
    throw std::runtime_error("Failed to convert InterfaceConfig to YANG node");

  // Wrap in YangData and send via edit-config (merge operation)
  YangData data(node);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Merge);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr,
                       c->session().yangContext());
}

void NetconfConfigurationManager::DestroyInterface(
    const std::string &name) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  YangContext yctx = c->session().yangContext();
  if (!yctx.get())
    throw YangError(yctx);

  // Find and delete the interface using xpath
  std::string xpath =
      std::format("/ietf-interfaces:interfaces/interface[name='{}']", name);
  struct lyd_node *node = nullptr;
  if (lyd_new_path(nullptr, yctx.get(), xpath.c_str(), nullptr,
                   LYD_NEW_PATH_UPDATE, &node) != LY_SUCCESS)
    throw YangError(yctx);

  // Add operation="delete" metadata
  struct lyd_meta *meta = nullptr;
  if (lyd_new_meta(nullptr, node, "ietf-netconf:operation", "delete", false,
                   &meta) != LY_SUCCESS) {
    lyd_free_all(node);
    throw YangError(yctx);
  }

  // Wrap in YangData and send via edit-config
  YangData data(node);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Delete);

  if (!reply || reply->isError()) {
    lyd_free_all(node);
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr, yctx);
  }
}

void NetconfConfigurationManager::RemoveInterfaceAddress(
    const std::string &ifname, const std::string &addr) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  YangContext yctx = c->session().yangContext();
  if (!yctx.get())
    throw YangError(yctx);

  // Remove a specific address using xpath with delete operation
  std::string xpath =
      std::format("/ietf-interfaces:interfaces/interface[name='{}']/ip:ipv4/"
                  "ip:address[ip='{}']",
                  ifname, addr);
  struct lyd_node *node = nullptr;
  if (lyd_new_path(nullptr, yctx.get(), xpath.c_str(), nullptr,
                   LYD_NEW_PATH_UPDATE, &node) != LY_SUCCESS)
    throw YangError(yctx);

  // Add operation="delete" metadata
  struct lyd_meta *meta = nullptr;
  if (lyd_new_meta(nullptr, node, "ietf-netconf:operation", "delete", false,
                   &meta) != LY_SUCCESS) {
    lyd_free_all(node);
    throw YangError(yctx);
  }

  // Wrap in YangData and send via edit-config
  YangData data(node);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Remove);

  if (!reply || reply->isError()) {
    lyd_free_all(node);
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr, yctx);
  }
}

void NetconfConfigurationManager::RemoveInterfaceGroup(
    const std::string & /*ifname*/, const std::string & /*group*/) const {
  // Group removal not supported via ietf-interfaces YANG model
  // This would require vendor-specific extensions
  throw NotImplementedError();
}

bool NetconfConfigurationManager::InterfaceExists(std::string_view name) const {
  Client *c = Client::instance();
  if (!c)
    return false;

  try {
    auto ifs = GetInterfaces();
    for (const auto &i : ifs) {
      if (i.name == name)
        return true;
    }
    return false;
  } catch (...) {
    return false;
  }
}

std::vector<std::string>
NetconfConfigurationManager::GetInterfaceAddresses(const std::string &ifname,
                                                   int family) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  auto ifs = GetInterfaces();
  for (const auto &i : ifs) {
    if (i.name == ifname) {
      std::vector<std::string> addrs;
      if (family == AF_INET || family == AF_UNSPEC) {
        if (i.address && i.address->family() == AddressFamily::IPv4)
          addrs.push_back(i.address->toString());
        for (const auto &a : i.aliases) {
          if (a->family() == AddressFamily::IPv4)
            addrs.push_back(a->toString());
        }
      }
      if (family == AF_INET6 || family == AF_UNSPEC) {
        if (i.address && i.address->family() == AddressFamily::IPv6)
          addrs.push_back(i.address->toString());
        for (const auto &a : i.aliases) {
          if (a->family() == AddressFamily::IPv6)
            addrs.push_back(a->toString());
        }
      }
      return addrs;
    }
  }
  return {};
}

void NetconfConfigurationManager::CreateVrf(const VRFConfig & /*vrf*/) const {}
void NetconfConfigurationManager::DeleteVrf(
    const std::string & /*name*/) const {}

std::vector<VRFConfig> NetconfConfigurationManager::GetVrfs() const {
  return {};
}

std::vector<VRFConfig> NetconfConfigurationManager::GetVrfs() const {
  return {};
}
