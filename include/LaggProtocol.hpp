/**
 * @file LaggProtocol.hpp
 * @brief LAGG protocol enumeration
 */

#pragma once

/**
 * @brief LAGG protocol types
 */
enum class LaggProtocol {
  LACP,        ///< Link Aggregation Control Protocol
  FAILOVER,    ///< Failover mode
  LOADBALANCE, ///< Load balancing mode
  NONE         ///< No protocol
};
