/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file InterfaceType.hpp
 * @brief Network interface type enumeration
 */

#pragma once

#include <string>

/**
 * @brief Network interface types
 *
 * Covers common physical and virtual interface types.
 */
enum class InterfaceType {
  Unknown,
  Loopback,
  Ethernet,
  Wireless,
  Bridge,
  Lagg,
  VLAN,
  PPP,
  Gif,
  Tun,
  GRE,
  VXLAN,
  IPsec,
  Ipip,
  FDDI,
  TokenRing,
  ATM,
  Epair,
  Carp,
  Tap,
  SixToFour,
  Ovpn,
  Pflog,
  Pfsync,
  WireGuard,
  Enc,
  VRF, // L3MDev on Linux Systems, but also support Layer 3 addressing
  Other,
};

/// Convert an InterfaceType enum value to a human-readable string.
inline std::string interfaceTypeToString(InterfaceType t) {
  switch (t) {
  case InterfaceType::Unknown:
    return "Unknown";
  case InterfaceType::Loopback:
    return "Loopback";
  case InterfaceType::Ethernet:
    return "Ethernet";
  case InterfaceType::Wireless:
    return "Wireless";
  case InterfaceType::Bridge:
    return "Bridge";
  case InterfaceType::Lagg:
    return "LinkAggregate";
  case InterfaceType::VLAN:
    return "VLAN";
  case InterfaceType::PPP:
    return "PointToPoint";
  case InterfaceType::Gif:
    return "GenericTunnel";
  case InterfaceType::Tun:
    return "Tun";
  case InterfaceType::GRE:
    return "GRE";
  case InterfaceType::VXLAN:
    return "VXLAN";
  case InterfaceType::IPsec:
    return "IPsec";
  case InterfaceType::FDDI:
    return "FDDI";
  case InterfaceType::TokenRing:
    return "TokenRing";
  case InterfaceType::ATM:
    return "ATM";
  case InterfaceType::Epair:
    return "VirtualEthernet";
  case InterfaceType::Carp:
    return "CARP";
  case InterfaceType::Tap:
    return "TAP";
  case InterfaceType::SixToFour:
    return "SixToFour";
  case InterfaceType::Ovpn:
    return "OpenVPN";
  case InterfaceType::Pflog:
    return "PFLog";
  case InterfaceType::Pfsync:
    return "PFSync";
  case InterfaceType::WireGuard:
    return "WireGuard";
  case InterfaceType::Enc:
    return "IPSEC";
  case InterfaceType::VRF:
    return "VRF";
  case InterfaceType::Other:
    return "Other";
  default:
    return "Unknown";
  }
}

inline InterfaceType interfaceTypeFromString(const std::string &s) {
  if (s == "ethernet")
    return InterfaceType::Ethernet;
  if (s == "lo")
    return InterfaceType::Loopback;
  if (s == "ppp")
    return InterfaceType::PPP;
  if (s == "bridge")
    return InterfaceType::Bridge;
  if (s == "vlan")
    return InterfaceType::VLAN;
  if (s == "lagg")
    return InterfaceType::Lagg;
  if (s == "gif")
    return InterfaceType::Gif;
  if (s == "tun")
    return InterfaceType::Tun;
  if (s == "gre")
    return InterfaceType::GRE;
  if (s == "vxlan")
    return InterfaceType::VXLAN;
  if (s == "epair")
    return InterfaceType::Epair;
  if (s == "carp")
    return InterfaceType::Carp;
  if (s == "tap")
    return InterfaceType::Tap;
  if (s == "stf")
    return InterfaceType::SixToFour;
  if (s == "ovpn")
    return InterfaceType::Ovpn;
  if (s == "pflog")
    return InterfaceType::Pflog;
  if (s == "pfsync")
    return InterfaceType::Pfsync;
  if (s == "wg")
    return InterfaceType::WireGuard;
  if (s == "enc")
    return InterfaceType::Enc;
  if (s == "wlan")
    return InterfaceType::Wireless;
  return InterfaceType::Unknown;
}

/// Convert InterfaceType to the IANA `iana-if-type` identity name string.
inline std::string interfaceTypeToIanaIdentity(InterfaceType t) {
  switch (t) {
  case InterfaceType::Ethernet:
    return "iana-if-type:ethernetCsmacd";
  case InterfaceType::Loopback:
    return "iana-if-type:softwareLoopback";
  case InterfaceType::PPP:
    return "iana-if-type:ppp";
  case InterfaceType::Wireless:
    return "iana-if-type:ieee80211";
  case InterfaceType::Bridge:
    return "iana-if-type:bridge";
  case InterfaceType::Lagg:
    return "iana-if-type:ieee8023adLag";
  case InterfaceType::VLAN:
    return "iana-if-type:l2vlan";
  case InterfaceType::Gif:
    return "iana-tunnel-type:gre";
  case InterfaceType::Tun:
    return "iana-tunnel-type:other";
  case InterfaceType::GRE:
    return "iana-tunnel-type:gre";
  case InterfaceType::VXLAN:
    return "iana-tunnel-type:other";
  case InterfaceType::IPsec:
    return "iana-tunnel-type:ipsectunnelmode";
  case InterfaceType::Epair:
    return "iana-if-type:ethernetCsmacd";
  case InterfaceType::Carp:
    return "iana-if-type:ethernetCsmacd";
  case InterfaceType::Tap:
    return "iana-if-type:ethernetCsmacd";
  case InterfaceType::SixToFour:
    return "iana-tunnel-type:sixtofour";
  case InterfaceType::Ovpn:
    return "iana-tunnel-type:other";
  case InterfaceType::VRF:
    return "ietf-network-instance";
  default:
    return "iana-if-type:other";
  }
}
