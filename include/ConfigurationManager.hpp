/**
 * @file ConfigurationManager.hpp
 * @brief Abstract configuration management interface
 */

#pragma once

#include "ConfigData.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Abstract base class for configuration storage and retrieval
 *
 * Provides an interface for setting and querying network configuration data.
 * Implementations can use in-memory storage, file-based persistence, or other
 * backends.
 */
class ConfigurationManager {
public:
  virtual ~ConfigurationManager() = default;

  // Enumeration API implemented by system-backed managers
  virtual std::vector<InterfaceConfig> GetInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<InterfaceConfig> GetInterfacesByGroup(const std::optional<VRFConfig> &vrf, std::string_view group) const = 0;
  virtual std::vector<BridgeInterfaceConfig> GetBridgeInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<LaggConfig> GetLaggInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<VLANConfig> GetVLANInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<TunnelConfig> GetTunnelInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<VirtualInterfaceConfig> GetVirtualInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<RouteConfig> GetStaticRoutes(const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<VRFConfig> GetNetworkInstances(const std::optional<int> &table = std::nullopt) const = 0;
};
