#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateWlan(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveWlan(
    const WlanInterfaceConfig & /*wlan*/) const {}
std::vector<WlanInterfaceConfig> NetconfConfigurationManager::GetWlanInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
