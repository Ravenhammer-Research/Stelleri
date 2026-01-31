/**
 * @file InterfaceConfig.hpp
 * @brief Network interface configuration structure
 */

#pragma once

#include "BridgeInterfaceConfig.hpp"
#include "IPNetwork.hpp"
#include "InterfaceType.hpp"
#include "LaggConfig.hpp"
#include "TunnelConfig.hpp"
#include "VLANConfig.hpp"
#include "VRFConfig.hpp"
#include "VirtualInterfaceConfig.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Complete configuration for a network interface
 *
 * Supports all interface types with type-specific configurations.
 * Optional fields allow for sparse configuration updates.
 */
struct InterfaceConfig {
  std::string name; ///< Interface name (e.g., em0, bridge0)
  InterfaceType type = InterfaceType::Unknown; ///< Interface type
  std::unique_ptr<IPNetwork> address; ///< Primary IP address with prefix
  std::vector<std::unique_ptr<IPNetwork>> aliases; ///< Additional IP addresses
  std::optional<VRFConfig> vrf;                    ///< VRF membership
  std::optional<uint32_t> flags;   ///< System flags (IFF_UP, IFF_RUNNING, etc.)
  std::optional<int> tunnel_vrf;   ///< FIB ID for tunnel routing lookups
  std::vector<std::string> groups; ///< Interface groups
  std::optional<BridgeInterfaceConfig>
      bridge;                         ///< Bridge-specific configuration
  std::optional<VLANConfig> vlan;     ///< VLAN-specific configuration
  std::optional<LaggConfig> lagg;     ///< Link aggregation configuration
  std::optional<TunnelConfig> tunnel; ///< Tunnel endpoint configuration
  std::optional<VirtualInterfaceConfig>
      virtual_config;     ///< Virtual interface (epair, tap) configuration
  std::optional<int> mtu; ///< Maximum Transmission Unit
};
