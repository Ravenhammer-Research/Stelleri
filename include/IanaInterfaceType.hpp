#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

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

} // namespace netconf
