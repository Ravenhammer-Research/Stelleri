#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "IetfInterfaces.hpp"
#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateVxlan(const std::string &name) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // VXLAN interfaces are created using the standard ietf-interfaces model
  // with type set to iana-if-type:vxlan (300)
  // Note: VXLAN requires ietf-vxlan YANG model for VNI and multicast group
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

  // Create the vxlan interface entry
  std::string iface_path =
      std::format("/ietf-interfaces:interfaces/interface[name='{}']", name);
  if (lyd_new_path(nullptr, yctx.get(), iface_path.c_str(), nullptr, 0,
                   &node) != LY_SUCCESS)
    throw YangError(yctx);

  // Set type to iana-if-type:vxlan
  std::string type_path = std::format(
      "/ietf-interfaces:interfaces/interface[name='{}']/type", name);
  if (lyd_new_path(nullptr, yctx.get(), type_path.c_str(), "iana-if-type:vxlan",
                   0, nullptr) != LY_SUCCESS)
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

void NetconfConfigurationManager::SaveVxlan(
    const VxlanInterfaceConfig &vxlan) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Convert to IetfInterfaces for the base interface config
  // Note: VXLAN-specific settings (VNI, group, src-port) would require
  // ietf-vxlan YANG model
  IetfInterfaces ii(vxlan);
  struct lyd_node *node = ii.toLydNode(c->session().yangContext());
  if (!node)
    throw std::runtime_error(
        "Failed to convert VxlanInterfaceConfig to YANG node");

  YangData data(node);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Merge);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr,
                       c->session().yangContext());
}

std::vector<VxlanInterfaceConfig>
NetconfConfigurationManager::GetVxlanInterfaces(
    const std::vector<InterfaceConfig> &bases) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Filter bases to only vxlan interfaces
  std::vector<VxlanInterfaceConfig> out;
  for (const auto &base : bases) {
    if (base.isVxlan()) {
      VxlanInterfaceConfig vif(base);
      out.push_back(vif);
    }
  }

  return out;
}
