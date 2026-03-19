#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

std::vector<PfsyncInterfaceConfig>
NetconfConfigurationManager::GetPfsyncInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}

void NetconfConfigurationManager::CreatePfsync(
    const std::string & /*name*/) const {}

void NetconfConfigurationManager::SavePfsync(
    const PfsyncInterfaceConfig & /*p*/) const {}

void NetconfConfigurationManager::DestroyPfsync(
    const std::string & /*name*/) const {}
