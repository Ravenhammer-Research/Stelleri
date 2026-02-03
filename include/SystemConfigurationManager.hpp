#pragma once

#include "BridgeInterfaceConfig.hpp"
#include "ConfigurationManager.hpp"
#include "InterfaceConfig.hpp"
#include "LaggConfig.hpp"
#include "RouteConfig.hpp"
#include "TunnelConfig.hpp"
#include "VLANConfig.hpp"
#include "VRFConfig.hpp"
#include "VirtualInterfaceConfig.hpp"
#include <optional>
#include <string_view>
#include <vector>

/**
 * @brief System configuration helpers (enumeration only)
 *
 * This simplified declaration provides only the static enumeration helpers
 * requested: methods that return smart-pointer-managed arrays of interface
 * configuration objects for each specific interface type.
 */
class SystemConfigurationManager : public ConfigurationManager {
public:
  // Instance overrides implement the abstract `ConfigurationManager` API.
  std::vector<InterfaceConfig> GetInterfaces(
      const std::optional<VRFConfig> &vrf = std::nullopt) const override;
  std::vector<InterfaceConfig>
  GetInterfacesByGroup(const std::optional<VRFConfig> &vrf,
                       std::string_view group) const override;
  std::vector<BridgeInterfaceConfig> GetBridgeInterfaces(
      const std::optional<VRFConfig> &vrf = std::nullopt) const override;
  std::vector<LaggConfig> GetLaggInterfaces(
      const std::optional<VRFConfig> &vrf = std::nullopt) const override;
  std::vector<VLANConfig> GetVLANInterfaces(
      const std::optional<VRFConfig> &vrf = std::nullopt) const override;
  std::vector<TunnelConfig> GetTunnelInterfaces(
      const std::optional<VRFConfig> &vrf = std::nullopt) const override;
  std::vector<VirtualInterfaceConfig> GetVirtualInterfaces(
      const std::optional<VRFConfig> &vrf = std::nullopt) const override;
  std::vector<RouteConfig> GetStaticRoutes(
      const std::optional<VRFConfig> &vrf = std::nullopt) const override;
  std::vector<RouteConfig>
  GetRoutes(const std::optional<VRFConfig> &vrf = std::nullopt) const override;
  std::vector<VRFConfig> GetNetworkInstances(
      const std::optional<int> &table = std::nullopt) const override;
};
