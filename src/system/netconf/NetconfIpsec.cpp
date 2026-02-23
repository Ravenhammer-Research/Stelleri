#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateIpsec(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveIpsec(
    const IpsecInterfaceConfig & /*ipsec*/) const {}
std::vector<IpsecInterfaceConfig>
NetconfConfigurationManager::GetIpsecInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
