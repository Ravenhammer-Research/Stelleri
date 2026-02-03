/**
 * @file ConfigurationManager.hpp
 * @brief Abstract configuration management interface
 */

#pragma once

#include "ConfigData.hpp"
#include "InterfaceConfig.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>
// Ensure configuration type declarations are available for method signatures
#include "BridgeInterfaceConfig.hpp"
#include "LaggConfig.hpp"
#include "RouteConfig.hpp"
#include "TunnelConfig.hpp"
#include "VLANConfig.hpp"
#include "VRFConfig.hpp"
#include "VirtualInterfaceConfig.hpp"

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
  virtual std::vector<InterfaceConfig>
  GetInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<InterfaceConfig>
  GetInterfacesByGroup(const std::optional<VRFConfig> &vrf,
                       std::string_view group) const = 0;
  virtual std::vector<BridgeInterfaceConfig> GetBridgeInterfaces(
      const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<LaggConfig> GetLaggInterfaces(
      const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<VLANConfig> GetVLANInterfaces(
      const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<TunnelConfig> GetTunnelInterfaces(
      const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<VirtualInterfaceConfig> GetVirtualInterfaces(
      const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<RouteConfig>
  GetStaticRoutes(const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<RouteConfig>
  GetRoutes(const std::optional<VRFConfig> &vrf = std::nullopt) const = 0;
  virtual std::vector<VRFConfig>
  GetNetworkInstances(const std::optional<int> &table = std::nullopt) const = 0;

  // Backwards-compatible helper: return interfaces as sliced `ConfigData`
  std::vector<ConfigData> getInterfaces() const {
    std::vector<ConfigData> out;
    auto ifs = GetInterfaces();
    out.reserve(ifs.size());
    for (auto &i : ifs) {
      ConfigData cd;
      cd.iface = std::make_shared<InterfaceConfig>(std::move(i));
      out.push_back(std::move(cd));
    }
    return out;
  }

  // Backwards-compatible helper: get single interface by name
  std::optional<ConfigData> getInterface(const std::string &name) const {
    auto ifs = GetInterfaces();
    for (auto &i : ifs) {
      if (i.name == name) {
        ConfigData cd;
        cd.iface = std::make_shared<InterfaceConfig>(std::move(i));
        return std::optional<ConfigData>(std::move(cd));
      }
    }
    return std::nullopt;
  }

  // Backwards-compatible helper: get routes as sliced ConfigData
  std::vector<ConfigData> getRoutes() const {
    std::vector<ConfigData> out;
    auto rs = GetRoutes();
    out.reserve(rs.size());
    for (auto &r : rs) {
      ConfigData cd;
      cd.route = std::make_shared<RouteConfig>(std::move(r));
      out.push_back(std::move(cd));
    }
    return out;
  }
};
