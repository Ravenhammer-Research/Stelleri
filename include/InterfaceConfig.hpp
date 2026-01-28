#pragma once

#include "BridgeInterfaceConfig.hpp"
#include "IPNetwork.hpp"
#include "InterfaceType.hpp"
#include "LaggConfig.hpp"
#include "VLANConfig.hpp"
#include "VRFConfig.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct InterfaceConfig {
  std::string name;
  InterfaceType type = InterfaceType::Unknown;
  std::unique_ptr<IPNetwork>
      address; // primary address as a network (host mask if needed)
  std::vector<std::unique_ptr<IPNetwork>> aliases; // additional addresses
  std::optional<VRFConfig> vrf;
  // system flags (e.g., IFF_UP, IFF_LOOPBACK) — optional to avoid implicit
  // defaults
  std::optional<uint32_t> flags;
  // optional VRF used for tunneling
  std::optional<VRFConfig> tunnel_vrf;
  // groups that this interface belongs to (e.g., network namespaces/groups)
  std::vector<std::string> groups;
  // optional bridge-specific configuration when this interface is a bridge
  std::optional<BridgeInterfaceConfig> bridge;
  // optional VLAN configuration
  std::optional<VLANConfig> vlan;
  // optional link-aggregation configuration
  std::optional<LaggConfig> lagg;
  // MTU is optional; avoid defaulting to 1500 which may be incorrect
  std::optional<int> mtu;
};
