#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "NetconfConfigurationManager.hpp"

// Policy/ACl operations require a custom YANG model as there's no standard
// ietf-access-control-list that's widely implemented. The policy definitions
// would need to be defined in a vendor-specific or project-specific YANG
// module.

std::vector<PolicyConfig> NetconfConfigurationManager::GetPolicies(
    const std::optional<uint32_t> & /*acl_filter*/) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Policy filtering not yet implemented - would require custom YANG model
  return {};
}

void NetconfConfigurationManager::SetPolicy(const PolicyConfig & /*pc*/) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Policy setting requires a custom YANG module for ACL definitions
  // This is currently a stub - implement when custom YANG model is available
}

void NetconfConfigurationManager::DeletePolicy(
    const PolicyConfig & /*pc*/) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Policy deletion requires a custom YANG module for ACL definitions
  // This is currently a stub - implement when custom YANG model is available
}
