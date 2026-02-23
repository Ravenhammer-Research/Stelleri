#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateEpair(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveEpair(
    const EpairInterfaceConfig & /*vic*/) const {}
std::vector<EpairInterfaceConfig>
NetconfConfigurationManager::GetEpairInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
