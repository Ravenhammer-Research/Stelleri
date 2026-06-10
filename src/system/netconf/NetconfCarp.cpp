#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "IetfInterfaces.hpp"
#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::SaveCarp(
    const CarpInterfaceConfig &carp) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Convert to IetfInterfaces for the base interface config
  // Note: CARP-specific settings (vhid, advbase, advskew) would require a
  // custom YANG model RFC 3768 defines Common Address Redundancy Protocol
  // (CARP)
  IetfInterfaces ii(carp);
  struct lyd_node *node = ii.toLydNode(c->session().yangContext());
  if (!node)
    throw std::runtime_error(
        "Failed to convert CarpInterfaceConfig to YANG node");

  YangData data(node);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Merge);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr,
                       c->session().yangContext());
}

std::vector<CarpInterfaceConfig> NetconfConfigurationManager::GetCarpInterfaces(
    const std::vector<InterfaceConfig> &bases) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Filter bases to only carp interfaces
  std::vector<CarpInterfaceConfig> out;
  for (const auto &base : bases) {
    if (base.isCarp()) {
      CarpInterfaceConfig cif(base);
      out.push_back(cif);
    }
  }

  return out;
}
