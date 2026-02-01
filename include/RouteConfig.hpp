/**
 * @file RouteConfig.hpp
 * @brief Route configuration structure
 */

#pragma once

#include "ConfigData.hpp"
#include <optional>
#include <string>

/**
 * @brief Configuration for a routing table entry
 */
class RouteConfig : public ConfigData {
public:
  std::string prefix;                 ///< Destination prefix in CIDR notation
  std::optional<std::string> nexthop; ///< Next-hop IP address
  std::optional<std::string> iface;   ///< Outgoing interface name
  std::optional<std::string> vrf;     ///< VRF name for route
  bool blackhole = false;             ///< Blackhole route (drop packets)
  bool reject = false;                ///< Reject route (send ICMP unreachable)
  
  // Persist route configuration (no-op for now)
  void save() const override {}
};
