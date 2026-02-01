/**
 * @file VRFConfig.hpp
 * @brief VRF (Virtual Routing and Forwarding) configuration
 */

#pragma once

#include "ConfigData.hpp"
#include <optional>
#include <string>

/**
 * @brief VRF configuration using FreeBSD FIB tables
 *
 * VRFs are implemented as FIB (Forwarding Information Base) tables in FreeBSD.
 */
class VRFConfig : public ConfigData {
public:
  std::string name;         ///< VRF name
  std::optional<int> table; ///< FIB table number (0-65535)

  // Persist VRF configuration (no-op placeholder)
  void save() const override {}
};
