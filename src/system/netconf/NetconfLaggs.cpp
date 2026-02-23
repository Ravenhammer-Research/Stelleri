#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateLagg(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveLagg(
    const LaggInterfaceConfig & /*lac*/) const {}
std::vector<LaggInterfaceConfig> NetconfConfigurationManager::GetLaggInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
