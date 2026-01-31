/**
 * @file BridgeInterfaceConfig.hpp
 * @brief Bridge interface configuration structure
 */

#pragma once

#include <optional>
#include <string>
#include <vector>

/**
 * @brief Configuration for a bridge member port
 */
struct BridgeMemberConfig {
  std::string name;             ///< Member interface name
  std::optional<int> priority;  ///< Port priority (0-255, default 128)
  std::optional<int> path_cost; ///< STP path cost (1-200000000, auto=0)
  bool stp = true;              ///< Enable STP on this port
  bool edge = false;            ///< Edge port (skip listening/learning)
  bool autoedge = true;         ///< Auto-detect edge ports
  bool ptp = false;             ///< Point-to-point link
  bool autoptp = true;          ///< Auto-detect PTP links
};

/**
 * @brief Configuration for bridge interfaces
 *
 * Stores bridge-specific settings including STP and member interfaces.
 */
struct BridgeInterfaceConfig {
  bool stp = false;                 ///< Spanning Tree Protocol enabled
  bool vlanFiltering = false;       ///< VLAN filtering enabled
  std::vector<std::string> members; ///< Member interface names (simple)
  std::vector<BridgeMemberConfig>
      member_configs;          ///< Detailed member configurations
  std::optional<int> priority; ///< Bridge priority (0-65535, default 32768)
  std::optional<int>
      hello_time; ///< STP hello time in seconds (1-10, default 2)
  std::optional<int>
      forward_delay; ///< STP forward delay in seconds (4-30, default 15)
  std::optional<int> max_age; ///< STP max age in seconds (6-40, default 20)
  std::optional<int>
      aging_time; ///< MAC address aging time in seconds (10-1000000)
  std::optional<int>
      max_addresses; ///< Maximum number of MAC addresses in cache
};
