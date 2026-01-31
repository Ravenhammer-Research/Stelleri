/**
 * @file LaggConfig.hpp
 * @brief Link aggregation (LAGG) configuration
 */

#pragma once

#include <string>
#include <vector>

/**
 * @brief LAGG protocol types
 */
enum class LaggProtocol {
  LACP,        ///< Link Aggregation Control Protocol
  FAILOVER,    ///< Failover mode
  LOADBALANCE, ///< Load balancing mode
  NONE         ///< No protocol
};

/**
 * @brief Configuration for link aggregation interfaces
 */
struct LaggConfig {
  LaggProtocol protocol =
      LaggProtocol::NONE;           ///< LAGG protocol (LACP, failover, etc.)
  std::vector<std::string> members; ///< Member port names
  std::optional<std::string>
      hash_policy;              ///< Hash policy (layer2, layer2+3, layer3+4)
  std::optional<int> lacp_rate; ///< LACP rate: 0=slow (30s), 1=fast (1s)
  std::optional<int> min_links; ///< Minimum number of active links
};
