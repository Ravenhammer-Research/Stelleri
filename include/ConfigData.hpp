/**
 * @file ConfigData.hpp
 * @brief Unified configuration data container
 */

#pragma once

#include "InterfaceConfig.hpp"
#include "RouteConfig.hpp"
#include "VRFConfig.hpp"

/**
 * @brief Container for configuration data of any type
 * 
 * Holds interface, route, or VRF configuration. Only one field is
 * populated at a time.
 */
struct ConfigData {
  std::optional<InterfaceConfig> iface;  ///< Interface configuration
  std::optional<RouteConfig> route;      ///< Route configuration
  std::optional<VRFConfig> vrf;          ///< VRF configuration
};
