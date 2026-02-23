#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::SaveVlan(
    const VlanInterfaceConfig & /*vlan*/) const {}
std::vector<VlanInterfaceConfig> NetconfConfigurationManager::GetVLANInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
