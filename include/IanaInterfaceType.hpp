#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "InterfaceType.hpp"

namespace netconf {

  inline std::string interfaceTypeToIanaIdentity(InterfaceType t) {
    switch (t) {
    case InterfaceType::Ethernet:
      return "ethernetCsmacd";
    case InterfaceType::Loopback:
      return "softwareLoopback";
    case InterfaceType::PPP:
      return "ppp";
    case InterfaceType::PointToPoint:
      return "propPointToPointSerial";
    case InterfaceType::Wireless:
      return "ieee80211";
    case InterfaceType::Bridge:
      return "bridge";
    case InterfaceType::Lagg:
      return "ieee8023adLag";
    case InterfaceType::VLAN:
      return "l2vlan";
    case InterfaceType::Gif:
      return "tunnel";
    case InterfaceType::Tun:
      return "tunnel";
    case InterfaceType::GRE:
      return "rfc1483";
    case InterfaceType::VXLAN:
      return "tunnel";
    case InterfaceType::IPsec:
      return "propVirtual";
    case InterfaceType::Epair:
      return "ethernetCsmacd";
    case InterfaceType::Carp:
      return "other";
    case InterfaceType::Tap:
      return "ethernetCsmacd";
    case InterfaceType::SixToFour:
      return "sixToFour";
    case InterfaceType::Ovpn:
      return "tunnel";
    default:
      return "other";
    }
  }

  inline InterfaceType ianaIdentityToInterfaceType(std::string_view id) {
    using sv = std::string_view;
    if (id == sv("ethernetCsmacd"))
      return InterfaceType::Ethernet;
    if (id == sv("softwareLoopback"))
      return InterfaceType::Loopback;
    if (id == sv("ppp"))
      return InterfaceType::PPP;
    if (id == sv("propPointToPointSerial"))
      return InterfaceType::PointToPoint;
    if (id == sv("ieee80211"))
      return InterfaceType::Wireless;
    if (id == sv("bridge"))
      return InterfaceType::Bridge;
    if (id == sv("ieee8023adLag"))
      return InterfaceType::Lagg;
    if (id == sv("l2vlan"))
      return InterfaceType::VLAN;
    if (id == sv("tunnel"))
      return InterfaceType::Tun; // generic tunnel -> prefer Tun
    if (id == sv("rfc1483"))
      return InterfaceType::GRE;
    if (id == sv("propVirtual"))
      return InterfaceType::IPsec;
    if (id == sv("other"))
      return InterfaceType::Carp;
    if (id == sv("sixToFour"))
      return InterfaceType::SixToFour;
    // Fallback: unknown identity
    return InterfaceType::Unknown;
  }
} // namespace netconf
