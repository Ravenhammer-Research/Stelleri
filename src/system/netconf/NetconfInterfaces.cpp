#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfConfigurationManager.hpp"

std::vector<InterfaceConfig> NetconfConfigurationManager::GetInterfaces(
    const std::optional<VRFConfig> & /*vrf*/) const {
  return {};
}

std::vector<InterfaceConfig> NetconfConfigurationManager::GetInterfacesByGroup(
    const std::optional<VRFConfig> & /*vrf*/,
    std::string_view /*group*/) const {
  return {};
}

void NetconfConfigurationManager::CreateInterface(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::SaveInterface(
    const InterfaceConfig & /*ic*/) const {}
void NetconfConfigurationManager::DestroyInterface(
    const std::string & /*name*/) const {}
void NetconfConfigurationManager::RemoveInterfaceAddress(
    const std::string & /*ifname*/, const std::string & /*addr*/) const {}
void NetconfConfigurationManager::RemoveInterfaceGroup(
    const std::string & /*ifname*/, const std::string & /*group*/) const {}
bool NetconfConfigurationManager::InterfaceExists(
    std::string_view /*name*/) const {
  return false;
}
std::vector<std::string> NetconfConfigurationManager::GetInterfaceAddresses(
    const std::string & /*ifname*/, int /*family*/) const {
  return {};
}

std::vector<VRFConfig> NetconfConfigurationManager::GetVrfs() const {
  return {};
}
