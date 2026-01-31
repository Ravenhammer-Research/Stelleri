/**
 * @file TunnelConfig.hpp
 * @brief Tunnel interface configuration
 */

#pragma once

#include <optional>
#include <string>

/**
 * @brief Configuration for tunnel interfaces (GIF, GRE, etc.)
 */
struct TunnelConfig {
  std::optional<std::string> source;       ///< Tunnel source address
  std::optional<std::string> destination;  ///< Tunnel destination address
  std::optional<int> ttl;                  ///< Tunnel TTL
  std::optional<int> tos;                  ///< Type of Service
};
