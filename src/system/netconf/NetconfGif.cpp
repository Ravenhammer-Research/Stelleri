#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateGif(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveGif(
    const GifInterfaceConfig & /*gif*/) const {}
std::vector<GifInterfaceConfig> NetconfConfigurationManager::GetGifInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
