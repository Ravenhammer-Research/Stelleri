#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "IetfInterfaces.hpp"
#include "NetconfConfigurationManager.hpp"
#include "VlanInterfaceConfig.hpp"

void NetconfConfigurationManager::SaveVlan(
    const VlanInterfaceConfig &vlan) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Convert to IetfInterfaces for the base interface config
  // Note: VLAN-specific settings (parent interface, vlan-id) would require a
  // custom YANG model
  IetfInterfaces ii(vlan);
  struct lyd_node *node = ii.toLydNode(c->session().yangContext());
  if (!node)
    throw std::runtime_error(
        "Failed to convert VlanInterfaceConfig to YANG node");

  YangData data(node);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Merge);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr,
                       c->session().yangContext());
}

std::vector<VlanInterfaceConfig> NetconfConfigurationManager::GetVLANInterfaces(
    const std::vector<InterfaceConfig> &bases) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Filter bases to only vlan interfaces
  std::vector<VlanInterfaceConfig> out;
  for (const auto &base : bases) {
    if (base.isVlan()) {
      VlanInterfaceConfig vlc(base);
      out.push_back(vlc);
    }
  }

  return out;
}
