#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateVxlan(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveVxlan(
    const VxlanInterfaceConfig & /*vxlan*/) const {}
std::vector<VxlanInterfaceConfig>
NetconfConfigurationManager::GetVxlanInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
