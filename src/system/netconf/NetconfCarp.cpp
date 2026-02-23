#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

std::vector<CarpInterfaceConfig> NetconfConfigurationManager::GetCarpInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
void NetconfConfigurationManager::SaveCarp(
    const CarpInterfaceConfig & /*carp*/) const {}
