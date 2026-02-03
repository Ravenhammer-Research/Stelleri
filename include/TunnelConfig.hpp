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
               std::unique_ptr<IPAddress> destination, std::optional<int> ttl,
               std::optional<int> tos);

  TunnelType type = TunnelType::UNKNOWN; ///< Tunnel implementation/type
  std::unique_ptr<IPAddress>
      source; ///< Tunnel source network/address (nullable)
  std::unique_ptr<IPAddress>
      destination;        ///< Tunnel destination network/address (nullable)
  std::optional<int> ttl; ///< Tunnel TTL
  std::optional<int> tos; ///< Type of Service

  // Copy semantics: clone underlying IPNetwork objects
  TunnelConfig(const TunnelConfig &o)
      : InterfaceConfig(o), type(o.type), ttl(o.ttl), tos(o.tos) {
    if (o.source)
      source = o.source->clone();
    if (o.destination)
      destination = o.destination->clone();
  }

  TunnelConfig &operator=(const TunnelConfig &o) {
    if (this == &o)
      return *this;
    InterfaceConfig::operator=(o);
    type = o.type;
    ttl = o.ttl;
    tos = o.tos;
    if (o.source)
      source = o.source->clone();
    else
      source.reset();
    if (o.destination)
      destination = o.destination->clone();
    else
      destination.reset();
    return *this;
  }

public:
  // Allow moving
  TunnelConfig(TunnelConfig &&) = default;
  TunnelConfig &operator=(TunnelConfig &&) = default;

  void save() const override;

private:
  void create() const;
};
