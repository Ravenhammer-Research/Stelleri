/**
 * @file VLANConfig.hpp
 * @brief VLAN interface configuration
 */

#pragma once

#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Configuration for VLAN interfaces
 * 
 * Stores VLAN ID and parent interface relationship.
 */
struct VLANConfig {
  uint16_t id = 0;                          ///< VLAN ID (1-4094)
  std::optional<std::string> name;          ///< Optional VLAN name
  std::optional<std::string> parent;        ///< Parent interface (e.g., "em0" for em0.100)
  std::optional<int> pcp;                   ///< Priority Code Point (0-7)
};
