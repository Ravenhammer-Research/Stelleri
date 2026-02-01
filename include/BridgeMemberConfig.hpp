/**
 * @file BridgeMemberConfig.hpp
 * @brief Bridge member port configuration
 */

#pragma once

#include <optional>
#include <string>

/**
 * @brief Configuration for a bridge member port
 */
class BridgeMemberConfig {
public:
  std::string name;             ///< Member interface name
  std::optional<int> priority;  ///< Port priority (0-255, default 128)
  std::optional<int> path_cost; ///< STP path cost (1-200000000, auto=0)
  bool stp = true;              ///< Enable STP on this port
  bool edge = false;            ///< Edge port (skip listening/learning)
  bool autoedge = true;         ///< Auto-detect edge ports
  bool ptp = false;             ///< Point-to-point link
  bool autoptp = true;          ///< Auto-detect PTP links
};
