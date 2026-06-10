#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "IPNetwork.hpp"
#include "NetconfConfigurationManager.hpp"

std::vector<ArpConfig> NetconfConfigurationManager::GetArpEntries(
    const std::optional<std::unique_ptr<IPNetwork>> & /*ip_filter*/,
    const std::optional<std::string> & /*iface*/) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Note: ARP entries don't have a standard IETF YANG model.
  // A custom YANG module (e.g., stelleri-arp) would need to be defined
  // to represent ARP table entries for NETCONF operations.

  return {};
}

bool NetconfConfigurationManager::SetArpEntry(
    const std::unique_ptr<IPNetwork> &ip, const std::string &mac,
    const std::optional<std::string> &iface, bool temp, bool pub) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Note: ARP entry modification requires a custom YANG model.
  // The ietf-arp module (if available) has restrictions on write operations
  // in many implementations. A vendor-specific or custom module like
  // stelleri-arp would be needed for programmatic ARP table manipulation.

  throw NotImplementedError();
}

bool NetconfConfigurationManager::DeleteArpEntry(
    const std::unique_ptr<IPNetwork> &ip,
    const std::optional<std::string> &iface) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Note: ARP entry deletion requires a custom YANG model.
  // See SetArpEntry() for details on the YANG modeling requirements.

  throw NotImplementedError();
}
