#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateTap(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveTap(
    const TapInterfaceConfig & /*tap*/) const {}
