/**
 * @file VirtualInterfaceConfig.hpp
 * @brief Virtual interface configuration (epair, tap, etc.)
 */

#pragma once

#include "InterfaceConfig.hpp"
#include <optional>
#include <string>

/**
 * @brief Configuration for virtual interfaces
 *
 * Covers epair (virtual ethernet pairs), tap, and similar virtual interfaces.
 * epair interfaces come in pairs (e.g., epair0a and epair0b) connected
 * back-to-back.
 */
class VirtualInterfaceConfig : public InterfaceConfig {
public:
  VirtualInterfaceConfig() = default;
  VirtualInterfaceConfig(const InterfaceConfig &base);
  VirtualInterfaceConfig(const InterfaceConfig &base,
                         std::optional<std::string> peer,
                         std::optional<int> rdomain, bool promiscuous);

  std::optional<std::string> peer; ///< Peer interface name (for epair pairs)
  std::optional<int> rdomain;      ///< Routing domain / FIB
  bool promiscuous = false;        ///< Promiscuous mode enabled

  void save() const override;

private:
  void create() const;
};
