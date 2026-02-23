#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

std::vector<ArpConfig> NetconfConfigurationManager::GetArpEntries(
    const std::optional<std::string> & /*ip*/,
    const std::optional<std::string> & /*iface*/) const {
  return {};
}

bool NetconfConfigurationManager::SetArpEntry(
    const std::string & /*ip*/, const std::string & /*mac*/,
    const std::optional<std::string> & /*iface*/, bool /*temp*/,
    bool /*pub*/) const {
  return false;
}

bool NetconfConfigurationManager::DeleteArpEntry(
    const std::string & /*ip*/,
    const std::optional<std::string> & /*iface*/) const {
  return false;
}
