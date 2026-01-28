#pragma once

#include <net/if.h>
#include <string>

enum class InterfaceType {
  Unknown,
  Loopback,
  Ethernet,
  PointToPoint,
  Wireless,
  Bridge,
  VLAN,
  PPP,
  Tunnel,
  FDDI,
  TokenRing,
  ATM,
  Virtual,
  Other,
};

inline std::string to_string(InterfaceType t) {
  switch (t) {
  case InterfaceType::Loopback:
    return "loopback";
  case InterfaceType::Ethernet:
    return "ethernet";
  case InterfaceType::PointToPoint:
    return "point-to-point";
  case InterfaceType::Wireless:
    return "wireless";
  case InterfaceType::Bridge:
    return "bridge";
  case InterfaceType::VLAN:
    return "vlan";
  case InterfaceType::PPP:
    return "ppp";
  case InterfaceType::Tunnel:
    return "tunnel";
  case InterfaceType::FDDI:
    return "fddi";
  case InterfaceType::TokenRing:
    return "token-ring";
  case InterfaceType::ATM:
    return "atm";
  case InterfaceType::Virtual:
    return "virtual";
  case InterfaceType::Other:
    return "other";
  default:
    return "unknown";
  }
}

// `detectInterfaceType` moved to the system implementation where `struct
// ifaddrs` is available so link-layer address families can be examined
// per-platform.
