#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

std::vector<RouteConfig> NetconfConfigurationManager::GetStaticRoutes(
    const std::optional<VRFConfig> & /*vrf*/) const {
  return {};
}
std::vector<RouteConfig> NetconfConfigurationManager::GetRoutes(
    const std::optional<VRFConfig> & /*vrf*/) const {
  return {};
}
void NetconfConfigurationManager::AddRoute(
    const RouteConfig & /*route*/) const {}
void NetconfConfigurationManager::DeleteRoute(
    const RouteConfig & /*route*/) const {}
