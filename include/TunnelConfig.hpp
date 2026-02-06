/**
 * @file TunnelConfig.hpp
 * @brief Tunnel interface configuration
 */

#pragma once

#include "IPAddress.hpp"
#include "InterfaceConfig.hpp"
#include "TunnelType.hpp"
#include <memory>
#include <optional>
#include <string>

/**
 * @brief Configuration for tunnel interfaces (GIF, GRE, etc.)
 */
/**
 * @brief Configuration for tunnel interfaces (GIF, GRE, etc.)
 */
class TunnelConfig : public InterfaceConfig {
public:
  TunnelConfig() = default;
  TunnelConfig(const InterfaceConfig &base);
  TunnelConfig(const InterfaceConfig &base, TunnelType type,
               std::unique_ptr<IPAddress> source,
               std::unique_ptr<IPAddress> destination);

  TunnelType type = TunnelType::UNKNOWN; ///< Tunnel implementation/type
  std::unique_ptr<IPAddress>
      source; ///< Tunnel source network/address (nullable)
  std::unique_ptr<IPAddress>
      destination; ///< Tunnel destination network/address (nullable)
  // TTL and TOS removed from TunnelConfig; managed elsewhere if needed
  std::optional<int> tunnel_vrf; ///< Tunnel-specific VRF/FIB (TunFIB)

  // Copy semantics: clone underlying IPNetwork objects
  TunnelConfig(const TunnelConfig &o)
      : InterfaceConfig(o), type(o.type), tunnel_vrf(o.tunnel_vrf) {
    if (o.source)
      source = o.source->clone();
    if (o.destination)
      destination = o.destination->clone();
  }

public:
  void save() const override;

private:
  void create() const;
};
