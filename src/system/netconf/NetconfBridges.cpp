#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateBridge(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveBridge(
    const BridgeInterfaceConfig & /*bic*/) const {}
std::vector<std::string> NetconfConfigurationManager::GetBridgeMembers(
    const std::string & /*name*/) const {
  return {};
}
std::vector<BridgeInterfaceConfig>
NetconfConfigurationManager::GetBridgeInterfaces(
    const std::vector<InterfaceConfig> & /*bases*/) const {
  return {};
}
