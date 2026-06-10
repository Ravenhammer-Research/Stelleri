#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "IetfInterfaces.hpp"
#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateWlan(const std::string &name) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // WLAN interfaces are created using the standard ietf-interfaces model
  // with type set to iana-if-type:ethernetCsmacd (6)
  // IEEE 802.11 wireless interfaces appear as ethernet devices at the link
  // layer
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

  // Create the wlan interface entry
  std::string iface_path =
      std::format("/ietf-interfaces:interfaces/interface[name='{}']", name);
  if (lyd_new_path(nullptr, yctx.get(), iface_path.c_str(), nullptr, 0,
                   &node) != LY_SUCCESS)
    throw YangError(yctx);

  // Set type to iana-if-type:ethernetCsmacd
  std::string type_path = std::format(
      "/ietf-interfaces:interfaces/interface[name='{}']/type", name);
  if (lyd_new_path(nullptr, yctx.get(), type_path.c_str(),
                   "iana-if-type:ethernetCsmacd", 0, nullptr) != LY_SUCCESS)
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

void NetconfConfigurationManager::SaveWlan(
    const WlanInterfaceConfig &wlan) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Convert to IetfInterfaces for the base interface config
  // Note: WLAN-specific settings (SSID, mode, encryption) would require a
  // custom YANG model
  IetfInterfaces ii(wlan);
  struct lyd_node *node = ii.toLydNode(c->session().yangContext());
  if (!node)
    throw std::runtime_error(
        "Failed to convert WlanInterfaceConfig to YANG node");

  YangData data(node);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Merge);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr,
                       c->session().yangContext());
}

std::vector<WlanInterfaceConfig> NetconfConfigurationManager::GetWlanInterfaces(
    const std::vector<InterfaceConfig> &bases) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Filter bases to only wlan interfaces
  std::vector<WlanInterfaceConfig> out;
  for (const auto &base : bases) {
    if (base.isWlan()) {
      WlanInterfaceConfig wif(base);
      out.push_back(wif);
    }
  }

  return out;
}
