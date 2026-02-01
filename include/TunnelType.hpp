/**
 * @file TunnelType.hpp
 * @brief Tunnel interface type enumeration
 */

#pragma once

#include <string>

/**
 * @brief Types of tunnel interfaces supported by NetCLI
 */
enum class TunnelType {
  GIF,     ///< Generic tunnel (GIF)
  GRE,     ///< GRE tunnel
  IPIP,    ///< IP-in-IP
  SIT,     ///< IPv6-in-IPv4 (SIT)
  UNKNOWN, ///< Unknown/unspecified tunnel type
};

inline std::string to_string(TunnelType t) {
  switch (t) {
  case TunnelType::GIF:
    return "gif";
  case TunnelType::GRE:
    return "gre";
  case TunnelType::IPIP:
    return "ipip";
  case TunnelType::SIT:
    return "sit";
  case TunnelType::UNKNOWN:
    return "unknown";
  default:
    return "unknown";
  }
}
