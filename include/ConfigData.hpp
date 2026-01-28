#pragma once

#include "InterfaceConfig.hpp"
#include "RouteConfig.hpp"
#include "VRFConfig.hpp"

// Simple wrapper; expand with std::variant when needed
struct ConfigData {
  std::optional<InterfaceConfig> iface;
  std::optional<RouteConfig> route;
  std::optional<VRFConfig> vrf;
};
