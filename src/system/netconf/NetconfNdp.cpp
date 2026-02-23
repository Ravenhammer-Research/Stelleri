#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

std::vector<NdpConfig> NetconfConfigurationManager::GetNdpEntries(
    const std::optional<std::string> & /*ip*/,
    const std::optional<std::string> & /*iface*/) const {
  return {};
}

bool NetconfConfigurationManager::SetNdpEntry(
    const std::string & /*ip*/, const std::string & /*mac*/,
    const std::optional<std::string> & /*iface*/, bool /*temp*/) const {
  return false;
}

bool NetconfConfigurationManager::DeleteNdpEntry(
    const std::string & /*ip*/,
    const std::optional<std::string> & /*iface*/) const {
  return false;
}
