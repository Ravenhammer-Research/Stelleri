/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 */

#include "WireGuardInterfaceConfig.hpp"
#include <stdexcept>

void WireGuardInterfaceConfig::create(ConfigurationManager &mgr) const {
  if (name.empty())
    throw std::runtime_error(
        "WireGuardInterfaceConfig has no interface name set");
  mgr.CreateInterface(name);
}

void WireGuardInterfaceConfig::save(ConfigurationManager &mgr) const {
  if (name.empty())
    throw std::runtime_error(
        "WireGuardInterfaceConfig has no interface name set");
  if (!InterfaceConfig::exists(mgr, name))
    create(mgr);
  mgr.SaveInterface(*this);
}

void WireGuardInterfaceConfig::destroy(ConfigurationManager &mgr) const {
  mgr.DestroyInterface(name);
}
