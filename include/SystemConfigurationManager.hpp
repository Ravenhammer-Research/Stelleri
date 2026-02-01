#pragma once

#include "BridgeInterfaceConfig.hpp"
#include "InterfaceConfig.hpp"
#include "VRFConfig.hpp"
#include <optional>
#include "LaggConfig.hpp"
#include "VLANConfig.hpp"
#include "TunnelConfig.hpp"
#include "VirtualInterfaceConfig.hpp"
#include <vector>
#include <string_view>
#include "RouteConfig.hpp"

/**
 * @brief System configuration helpers (enumeration only)
 *
 * This simplified declaration provides only the static enumeration helpers
 * requested: methods that return smart-pointer-managed arrays of interface
 * configuration objects for each specific interface type.
 */
class SystemConfigurationManager : public ConfigurationManager {
public:
	// Static helpers remain available for direct calls; instance methods
	// override the abstract `ConfigurationManager` API and delegate to the
	// static implementations for convenience.
	static std::vector<InterfaceConfig> GetInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt);
	static std::vector<InterfaceConfig> GetInterfacesByGroup(const std::optional<VRFConfig> &vrf, std::string_view group);
	static std::vector<BridgeInterfaceConfig> GetBridgeInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt);
	static std::vector<LaggConfig> GetLaggInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt);
	static std::vector<VLANConfig> GetVLANInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt);
	static std::vector<TunnelConfig> GetTunnelInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt);
	static std::vector<VirtualInterfaceConfig> GetVirtualInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt);
	static std::vector<RouteConfig> GetStaticRoutes(const std::optional<VRFConfig> &vrf = std::nullopt);
	static std::vector<VRFConfig> GetNetworkInstances(const std::optional<int> &table = std::nullopt);

	// Instance overrides delegate to the static implementations
	std::vector<InterfaceConfig> GetInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const override { return SystemConfigurationManager::GetInterfaces(vrf); }
	std::vector<InterfaceConfig> GetInterfacesByGroup(const std::optional<VRFConfig> &vrf, std::string_view group) const override { return SystemConfigurationManager::GetInterfacesByGroup(vrf, group); }
	std::vector<BridgeInterfaceConfig> GetBridgeInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const override { return SystemConfigurationManager::GetBridgeInterfaces(vrf); }
	std::vector<LaggConfig> GetLaggInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const override { return SystemConfigurationManager::GetLaggInterfaces(vrf); }
	std::vector<VLANConfig> GetVLANInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const override { return SystemConfigurationManager::GetVLANInterfaces(vrf); }
	std::vector<TunnelConfig> GetTunnelInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const override { return SystemConfigurationManager::GetTunnelInterfaces(vrf); }
	std::vector<VirtualInterfaceConfig> GetVirtualInterfaces(const std::optional<VRFConfig> &vrf = std::nullopt) const override { return SystemConfigurationManager::GetVirtualInterfaces(vrf); }
	std::vector<RouteConfig> GetStaticRoutes(const std::optional<VRFConfig> &vrf = std::nullopt) const override { return SystemConfigurationManager::GetStaticRoutes(vrf); }
	std::vector<VRFConfig> GetNetworkInstances(const std::optional<int> &table = std::nullopt) const override { return SystemConfigurationManager::GetNetworkInstances(table); }
};

