#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateOvpn(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveOvpn(
    const OvpnInterfaceConfig & /*ovpn*/) const {}
std::vector<OvpnInterfaceConfig> NetconfConfigurationManager::GetOvpnInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
