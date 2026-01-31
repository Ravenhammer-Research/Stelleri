/**
 * @file VirtualInterfaceConfig.hpp
 * @brief Virtual interface configuration (epair, tap, etc.)
 */

#pragma once

#include <optional>
#include <string>

/**
 * @brief Configuration for virtual interfaces
 *
 * Covers epair (virtual ethernet pairs), tap, and similar virtual interfaces.
 * epair interfaces come in pairs (e.g., epair0a and epair0b) connected
 * back-to-back.
 */
struct VirtualInterfaceConfig {
  std::optional<std::string> peer; ///< Peer interface name (for epair pairs)
  std::optional<int> rdomain;      ///< Routing domain / FIB
  bool promiscuous = false;        ///< Promiscuous mode enabled
};
