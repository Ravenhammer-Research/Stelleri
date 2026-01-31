#include "LaggTableFormatter.hpp"
#include "InterfaceConfig.hpp"
#include "InterfaceType.hpp"
#include "LaggConfig.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

static std::string protocolToString(LaggProtocol proto) {
  switch (proto) {
  case LaggProtocol::LACP:
    return "LACP";
  case LaggProtocol::FAILOVER:
    return "Failover";
  case LaggProtocol::LOADBALANCE:
    return "Load Balance";
  case LaggProtocol::NONE:
    return "None";
  default:
    return "Unknown";
  }
}

std::string LaggTableFormatter::format(
    const std::vector<ConfigData> &interfaces) const {
  if (interfaces.empty()) {
    return "No LAGG interfaces found.\n";
  }

  std::ostringstream oss;
  oss << "\nLAGG Interfaces\n";
  oss << std::string(80, '=') << "\n\n";

  for (const auto &cd : interfaces) {
    if (!cd.iface)
      continue;

    const auto &ic = *cd.iface;
    
    // Check if this is a LAGG interface (has lagg config)
    if (!ic.lagg)
      continue;

    oss << "LAGG: " << ic.name << "\n";

    const auto &lagg = *ic.lagg;
    
    oss << "  Protocol:       " << protocolToString(lagg.protocol) << "\n";
    
    if (lagg.hash_policy)
      oss << "  Hash Policy:    " << *lagg.hash_policy << "\n";
    if (lagg.lacp_rate)
      oss << "  LACP Rate:      " << (*lagg.lacp_rate == 1 ? "fast (1s)" : "slow (30s)") << "\n";
    if (lagg.min_links)
      oss << "  Min Links:      " << *lagg.min_links << "\n";

    if (!lagg.members.empty()) {
      oss << "  Members:\n";
      for (const auto &member : lagg.members) {
        oss << "    - " << member << "\n";
      }
    }

    if (ic.address) {
      oss << "  Address:        " << ic.address->toString() << "\n";
    }
    if (ic.mtu) {
      oss << "  MTU:            " << *ic.mtu << "\n";
    }
    
    oss << "\n";
  }

  return oss.str();
}
