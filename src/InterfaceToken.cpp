#include "InterfaceToken.hpp"
#include "BridgeTableFormatter.hpp"
#include "InterfaceFlags.hpp"
#include "InterfaceTableFormatter.hpp"
#include "LaggTableFormatter.hpp"
#include "SingleInterfaceSummaryFormatter.hpp"
#include "TunnelTableFormatter.hpp"
#include "VLANTableFormatter.hpp"
#include "VirtualTableFormatter.hpp"
#include <iostream>
#include <sstream>

InterfaceToken::InterfaceToken(InterfaceType t, std::string name)
    : type_(t), name_(std::move(name)) {}

std::string InterfaceToken::toString() const {
  std::string t;
  switch (type_) {
  case InterfaceType::Ethernet:
    t = "interfaces ethernet";
    break;
  case InterfaceType::Loopback:
    t = "interfaces loopback";
    break;
  case InterfaceType::PPP:
    t = "interfaces ppp";
    break;
  default:
    t = "interfaces";
    break;
  }
  return t + " " + name_;
}

std::vector<std::string> InterfaceToken::autoComplete(std::string_view) const {
  return {}; // suggestions could query ConfigurationManager; keep empty for
             // now
}

std::unique_ptr<Token> InterfaceToken::clone() const {
  return std::make_unique<InterfaceToken>(*this);
}

std::string InterfaceToken::renderTable(ConfigurationManager *mgr) const {
  if (!mgr)
    return "Error: No configuration manager\n";

  std::vector<ConfigData> interfaces;
  if (!name_.empty()) {
    // Get specific interface by name
    auto cdopt = mgr->getInterface(name_);
    if (cdopt) {
      interfaces.push_back(std::move(*cdopt));
    }
  } else if (type_ != InterfaceType::Unknown) {
    // Filter by type
    auto allIfaces = mgr->getInterfaces();
    for (auto &iface : allIfaces) {
      if (iface.iface && iface.iface->type == type_) {
        interfaces.push_back(std::move(iface));
      }
    }
  } else {
    // Get all interfaces
    interfaces = mgr->getInterfaces();
  }

  // Single interface: show as summary
  if (interfaces.size() == 1 && !name_.empty()) {
    SingleInterfaceSummaryFormatter formatter;
    return formatter.format(interfaces[0]);
  }

  // Multiple interfaces: check if all are the same specialized type
  bool allBridge = true;
  bool allLagg = true;
  bool allVlan = true;
  bool allTunnel = true;
  bool allVirtual = true;

  for (const auto &cd : interfaces) {
    if (!cd.iface)
      continue;
    if (cd.iface->type != InterfaceType::Bridge)
      allBridge = false;
    if (!cd.iface->lagg)
      allLagg = false;
    if (cd.iface->type != InterfaceType::VLAN)
      allVlan = false;
    if (cd.iface->type != InterfaceType::Tunnel)
      allTunnel = false;
    if (cd.iface->type != InterfaceType::Virtual)
      allVirtual = false;
  }

  // Use specialized formatter if all interfaces are the same type
  if (allBridge && !interfaces.empty()) {
    BridgeTableFormatter formatter;
    return formatter.format(interfaces);
  } else if (allLagg && !interfaces.empty()) {
    LaggTableFormatter formatter;
    return formatter.format(interfaces);
  } else if (allVlan && !interfaces.empty()) {
    VLANTableFormatter formatter;
    return formatter.format(interfaces);
  } else if (allTunnel && !interfaces.empty()) {
    TunnelTableFormatter formatter;
    return formatter.format(interfaces);
  } else if (allVirtual && !interfaces.empty()) {
    VirtualTableFormatter formatter;
    return formatter.format(interfaces);
  }

  // Fall back to general interface table formatter
  InterfaceTableFormatter formatter;
  return formatter.format(interfaces);
}

std::shared_ptr<InterfaceToken>
InterfaceToken::parseFromTokens(const std::vector<std::string> &tokens,
                                size_t start, size_t &next) {
  next = start + 1; // by default consume the 'interfaces' token
  if (start + 2 < tokens.size()) {
    const std::string &type = tokens[start + 1];
    const std::string &name = tokens[start + 2];
    InterfaceType itype = InterfaceType::Unknown;
    if (type == "ethernet")
      itype = InterfaceType::Ethernet;
    else if (type == "loopback")
      itype = InterfaceType::Loopback;
    else if (type == "ppp")
      itype = InterfaceType::PPP;
    else if (type == "bridge")
      itype = InterfaceType::Bridge;
    else if (type == "vlan")
      itype = InterfaceType::VLAN;
    else if (type == "lagg")
      itype = InterfaceType::Other; // LAGG doesn't have dedicated type
    else if (type == "tunnel" || type == "gif")
      itype = InterfaceType::Tunnel;
    else if (type == "epair")
      itype = InterfaceType::Virtual;
    if (itype != InterfaceType::Unknown) {
      next = start + 3;
      std::cerr << "[InterfaceToken] parseFromTokens: consumed tokens '" << type
                << "' '" << name << "' starting at " << start
                << " -> type=" << static_cast<int>(itype) << " name='" << name
                << "' next=" << next << "\n";
      return std::make_shared<InterfaceToken>(itype, name);
    }
  }
  // bare interfaces
  std::cerr << "[InterfaceToken] parseFromTokens: bare 'interfaces' at "
            << start << "\n";
  return std::make_shared<InterfaceToken>(InterfaceType::Unknown,
                                          std::string());
}
