#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateTun(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveTun(
    const TunInterfaceConfig & /*tun*/) const {}
std::vector<TunInterfaceConfig> NetconfConfigurationManager::GetTunInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
