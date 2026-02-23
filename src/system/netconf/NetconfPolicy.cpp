#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

std::vector<PolicyConfig> NetconfConfigurationManager::GetPolicies(
    const std::optional<uint32_t> & /*acl_filter*/) const {
  return {};
}
void NetconfConfigurationManager::SetPolicy(const PolicyConfig & /*pc*/) const {
}
void NetconfConfigurationManager::DeletePolicy(
    const PolicyConfig & /*pc*/) const {}
