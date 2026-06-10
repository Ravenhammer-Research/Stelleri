#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "IPNetwork.hpp"
#include "NetconfConfigurationManager.hpp"

std::vector<NdpConfig> NetconfConfigurationManager::GetNdpEntries(
    const std::optional<std::unique_ptr<IPNetwork>> & /*ip_filter*/,
    const std::optional<std::string> & /*iface*/) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Note: NDP entries don't have a standard IETF YANG model.
  // A custom YANG module (e.g., stelleri-ndp) would need to be defined
  // to represent NDP table entries for NETCONF operations.

  return {};
}

bool NetconfConfigurationManager::SetNdpEntry(
    const std::unique_ptr<IPNetwork> &ip, const std::string &mac,
    const std::optional<std::string> &iface, bool temp) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Note: NDP entry modification requires a custom YANG model.
  // See GetNdpEntries() for details on the YANG modeling requirements.

  throw NotImplementedError();
}

bool NetconfConfigurationManager::DeleteNdpEntry(
    const std::unique_ptr<IPNetwork> &ip,
    const std::optional<std::string> &iface) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Note: NDP entry deletion requires a custom YANG model.
  // See GetNdpEntries() for details on the YANG modeling requirements.

  throw NotImplementedError();
}
