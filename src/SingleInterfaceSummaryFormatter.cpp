#include "SingleInterfaceSummaryFormatter.hpp"
#include "InterfaceConfig.hpp"
#include "InterfaceFlags.hpp"
#include "InterfaceType.hpp"
#include "LaggConfig.hpp"
#include <sstream>

std::string
SingleInterfaceSummaryFormatter::format(const ConfigData &cd) const {
  if (!cd.iface)
    return "Interface not found.\n";

  const auto &ic = *cd.iface;
  std::ostringstream oss;

  oss << "Interface: " << ic.name << "\n";
  oss << "Type:      " << to_string(ic.type) << "\n";

  if (ic.flags) {
    std::string status = "-";
    if (*ic.flags & static_cast<uint32_t>(InterfaceFlag::RUNNING)) {
      status = "active";
    } else if (*ic.flags & static_cast<uint32_t>(InterfaceFlag::UP)) {
      status = "no-carrier";
    } else {
      status = "down";
    }
    oss << "Status:    " << status << "\n";
  }

  if (ic.mtu) {
    oss << "MTU:       " << *ic.mtu << "\n";
  }

  if (ic.address) {
    oss << "Address:   " << ic.address->toString() << "\n";
  }

  for (const auto &alias : ic.aliases) {
    oss << "           " << alias->toString() << "\n";
  }

  if (ic.vrf) {
    oss << "VRF:       " << ic.vrf->name << "\n";
  }

  if (ic.tunnel_vrf) {
    oss << "Tunnel VRF: " << *ic.tunnel_vrf << "\n";
  }

  // Show tunnel details
  if (ic.tunnel) {
    if (ic.tunnel->source)
      oss << "Tunnel Src: " << ic.tunnel->source->toString() << "\n";
    if (ic.tunnel->destination)
      oss << "Tunnel Dst: " << ic.tunnel->destination->toString() << "\n";
    if (ic.tunnel->ttl)
      oss << "Tunnel TTL: " << *ic.tunnel->ttl << "\n";
    if (ic.tunnel->tos)
      oss << "Tunnel TOS: " << *ic.tunnel->tos << "\n";
  }

  // Show bridge details
  if (ic.bridge) {
    oss << "Bridge STP: " << (ic.bridge->stp ? "enabled" : "disabled") << "\n";
    if (!ic.bridge->members.empty()) {
      oss << "Members:   ";
      bool first = true;
      for (const auto &m : ic.bridge->members) {
        if (!first)
          oss << ", ";
        oss << m;
        first = false;
      }
      oss << "\n";
    }
  }

  // Show LAGG details
  if (ic.lagg) {
    oss << "LAGG Proto: ";
    switch (ic.lagg->protocol) {
    case LaggProtocol::LACP:
      oss << "LACP";
      break;
    case LaggProtocol::FAILOVER:
      oss << "Failover";
      break;
    case LaggProtocol::LOADBALANCE:
      oss << "Load Balance";
      break;
    case LaggProtocol::NONE:
      oss << "None";
      break;
    }
    oss << "\n";

    if (!ic.lagg->members.empty()) {
      oss << "Members:   ";
      bool first = true;
      for (const auto &m : ic.lagg->members) {
        if (!first)
          oss << ", ";
        oss << m;
        first = false;
      }
      oss << "\n";
    }
  }

  // Show VLAN details
  if (ic.vlan) {
    oss << "VLAN ID:   " << ic.vlan->id << "\n";
    if (ic.vlan->parent)
      oss << "Parent:    " << *ic.vlan->parent << "\n";
    if (ic.vlan->pcp)
      oss << "PCP:       " << static_cast<int>(*ic.vlan->pcp) << "\n";
  }

  if (ic.flags) {
    oss << "Flags:     ";
    if (*ic.flags & static_cast<uint32_t>(InterfaceFlag::UP))
      oss << "UP ";
    if (*ic.flags & static_cast<uint32_t>(InterfaceFlag::BROADCAST))
      oss << "BROADCAST ";
    if (*ic.flags & static_cast<uint32_t>(InterfaceFlag::LOOPBACK))
      oss << "LOOPBACK ";
    if (*ic.flags & static_cast<uint32_t>(InterfaceFlag::POINTOPOINT))
      oss << "POINTOPOINT ";
    if (*ic.flags & static_cast<uint32_t>(InterfaceFlag::RUNNING))
      oss << "RUNNING ";
    if (*ic.flags & static_cast<uint32_t>(InterfaceFlag::MULTICAST))
      oss << "MULTICAST";
    oss << "\n";
  }

  return oss.str();
}
