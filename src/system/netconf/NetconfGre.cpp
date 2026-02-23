#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateGre(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveGre(
    const GreInterfaceConfig & /*gre*/) const {}
std::vector<GreInterfaceConfig> NetconfConfigurationManager::GetGreInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
