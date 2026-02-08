/*
 * WLAN system helper implementations
 */

#include "SystemConfigurationManager.hpp"
#include "WlanConfig.hpp"

#include <net/if.h>
#include <sys/sockio.h>

void SystemConfigurationManager::CreateWlan(const std::string &name) const {
  if (InterfaceConfig::exists(*this, name))
    return;
  cloneInterface(name, SIOCIFCREATE);
}

void SystemConfigurationManager::SaveWlan(const WlanConfig &wlan) const {
  if (wlan.name.empty())
    throw std::runtime_error("WlanConfig has no interface name set");

  if (!InterfaceConfig::exists(*this, wlan.name))
    CreateWlan(wlan.name);

  // Reuse generic interface save for addresses/mtu/flags
  SaveInterface(wlan);
}
