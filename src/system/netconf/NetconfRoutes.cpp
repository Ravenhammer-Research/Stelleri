#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "NetconfConfigurationManager.hpp"
#include "RouteConfig.hpp"

std::vector<RouteConfig> NetconfConfigurationManager::GetStaticRoutes(
    const std::optional<VRFConfig> & /*vrf*/) const {
  // Static routes would be retrieved via NETCONF get-config
  // For now, delegate to GetRoutes which can query the routing table
  return GetRoutes(vrf);
}

std::vector<RouteConfig> NetconfConfigurationManager::GetRoutes(
    const std::optional<VRFConfig> & /*vrf*/) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // TODO: Implement route retrieval via YANG model (ietf-routing or
  // vendor-specific) For now, this would require:
  // 1. Querying the ietf-routing module for static routes
  // 2. Parsing the response into RouteConfig objects

  return {};
}

void NetconfConfigurationManager::AddRoute(const RouteConfig &route) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // TODO: Implement route addition via NETCONF edit-config
  // This would require:
  // 1. Building the YANG node for the route (ietf-routing module)
  // 2. Sending via c->editConfig() with appropriate operation

  // Placeholder: throw for now to indicate not implemented
  throw NotImplementedError();
}

void NetconfConfigurationManager::DeleteRoute(const RouteConfig &route) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // TODO: Implement route deletion via NETCONF edit-config
  // This would require:
  // 1. Building the YANG node path for the route
  // 2. Sending delete operation via c->editConfig()

  // Placeholder: throw for now to indicate not implemented
  throw NotImplementedError();
}
