/*
 * TAP system helper implementations
 */

#include "SystemConfigurationManager.hpp"
#include "TapConfig.hpp"

#include <net/if.h>
#include <sys/sockio.h>

void SystemConfigurationManager::CreateTap(const std::string &name) const {
  if (InterfaceConfig::exists(*this, name))
    return;
  cloneInterface(name, SIOCIFCREATE);
}

void SystemConfigurationManager::SaveTap(const TapConfig &tap) const {
  if (tap.name.empty())
    throw std::runtime_error("TapConfig has no interface name set");

  if (!InterfaceConfig::exists(*this, tap.name))
    CreateTap(tap.name);

  // Use generic interface save for addresses/mtu/flags
  SaveInterface(tap);
}
