#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "IetfInterfaces.hpp"
#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreatePflog(const std::string &name) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // PFLOG interfaces are created using the standard ietf-interfaces model
  // with type set to iana-if-type:tunnel (131)
  // Note: pfLog interface for packet logging in OpenBSD/FreeBSD pf firewall
  YangContext yctx = c->session().yangContext();
  if (!yctx.get())
    throw YangError(yctx);

  struct lyd_node *node = nullptr;

  const struct lys_module *mod =
      ly_ctx_get_module_implemented(yctx.get(), "ietf-interfaces");
  if (!mod)
    mod = ly_ctx_load_module(const_cast<struct ly_ctx *>(yctx.get()),
                             "ietf-interfaces", nullptr, nullptr);
  if (!mod)
    throw std::runtime_error("ietf-interfaces module not found");

  // Create the pflog interface entry
  std::string iface_path =
      std::format("/ietf-interfaces:interfaces/interface[name='{}']", name);
  if (lyd_new_path(nullptr, yctx.get(), iface_path.c_str(), nullptr, 0,
                   &node) != LY_SUCCESS)
    throw YangError(yctx);

  // Set type to iana-if-type:tunnel
  std::string type_path = std::format(
      "/ietf-interfaces:interfaces/interface[name='{}']/type", name);
  if (lyd_new_path(nullptr, yctx.get(), type_path.c_str(),
                   "iana-if-type:tunnel", 0, nullptr) != LY_SUCCESS)
    throw YangError(yctx);

  // Add operation="create" metadata
  struct lyd_meta *meta = nullptr;
  if (lyd_new_meta(nullptr, node, "ietf-netconf:operation", "create", false,
                   &meta) != LY_SUCCESS)
    throw YangError(yctx);

  // Create container and insert node
  struct lyd_node *iface_data = nullptr;
  if (lyd_new_inner(nullptr, mod, "interfaces", 0, &iface_data) != LY_SUCCESS)
    throw YangError(yctx);
  lyd_insert_child(iface_data, node);

  YangData data(iface_data);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Create);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr, yctx);
}

void NetconfConfigurationManager::SavePflog(
    const PflogInterfaceConfig &p) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Convert to IetfInterfaces for the base interface config
  // Note: PFLOG-specific settings (log interface, filter rules) would require a
  // custom YANG model
  IetfInterfaces ii(p);
  struct lyd_node *node = ii.toLydNode(c->session().yangContext());
  if (!node)
    throw std::runtime_error(
        "Failed to convert PflogInterfaceConfig to YANG node");

  YangData data(node);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Merge);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr,
                       c->session().yangContext());
}

void NetconfConfigurationManager::DestroyPflog(const std::string &name) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Delete the pflog interface using ietf-interfaces model
  YangContext yctx = c->session().yangContext();
  if (!yctx.get())
    throw YangError(yctx);

  struct lyd_node *node = nullptr;

  const struct lys_module *mod =
      ly_ctx_get_module_implemented(yctx.get(), "ietf-interfaces");
  if (!mod)
    mod = ly_ctx_load_module(const_cast<struct ly_ctx *>(yctx.get()),
                             "ietf-interfaces", nullptr, nullptr);
  if (!mod)
    throw std::runtime_error("ietf-interfaces module not found");

  // Create the interface entry with delete operation
  std::string iface_path =
      std::format("/ietf-interfaces:interfaces/interface[name='{}']", name);
  if (lyd_new_path(nullptr, yctx.get(), iface_path.c_str(), nullptr, 0,
                   &node) != LY_SUCCESS)
    throw YangError(yctx);

  // Set type to iana-if-type:tunnel
  std::string type_path = std::format(
      "/ietf-interfaces:interfaces/interface[name='{}']/type", name);
  if (lyd_new_path(nullptr, yctx.get(), type_path.c_str(),
                   "iana-if-type:tunnel", 0, nullptr) != LY_SUCCESS)
    throw YangError(yctx);

  // Add operation="delete" metadata
  struct lyd_meta *meta = nullptr;
  if (lyd_new_meta(nullptr, node, "ietf-netconf:operation", "delete", false,
                   &meta) != LY_SUCCESS)
    throw YangError(yctx);

  // Create container and insert node
  struct lyd_node *iface_data = nullptr;
  if (lyd_new_inner(nullptr, mod, "interfaces", 0, &iface_data) != LY_SUCCESS)
    throw YangError(yctx);
  lyd_insert_child(iface_data, node);

  YangData data(iface_data);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Delete);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr, yctx);
}

std::vector<PflogInterfaceConfig>
NetconfConfigurationManager::GetPflogInterfaces(
    const std::vector<InterfaceConfig> &bases) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Filter bases to only pflog interfaces
  std::vector<PflogInterfaceConfig> out;
  for (const auto &base : bases) {
    if (base.isPflog()) {
      PflogInterfaceConfig pif(base);
      out.push_back(pif);
    }
  }

  return out;
}
