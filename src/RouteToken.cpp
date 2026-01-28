#include "RouteToken.hpp"

RouteToken::RouteToken(std::string prefix) : prefix_(std::move(prefix)) {}

std::string RouteToken::toString() const {
  return std::string("route ") + prefix_;
}

std::vector<std::string> RouteToken::autoComplete(std::string_view) const {
  return {"interface", "next-hop", "blackhole", "vrf"};
}

std::unique_ptr<Token> RouteToken::clone() const {
  auto r = std::make_unique<RouteToken>(prefix_);
  if (nexthop)
    r->nexthop = nexthop->clone();
  if (interface)
    r->interface = std::make_unique<InterfaceToken>(*interface);
  if (vrf)
    r->vrf = std::make_unique<VRFToken>(*vrf);
  r->blackhole = blackhole;
  return r;
}

void RouteToken::debugOutput(std::ostream &os) const {
  os << "[parser] parsed route: prefix='" << prefix_ << "'";
  if (nexthop)
    os << " nexthop='" << nexthop->toString() << "'";
  if (vrf)
    os << " vrf='" << vrf->name() << "'";
  if (interface)
    os << " interface='" << interface->name() << "'";
  if (blackhole)
    os << " blackhole=true";
  os << '\n';
}

size_t RouteToken::parseOptions(const std::vector<std::string> &tokens,
                                size_t start) {
  size_t j = start;
  while (j < tokens.size()) {
    const auto &opt = tokens[j];
    if (opt == "next-hop" && j + 1 < tokens.size()) {
      const std::string nh = tokens[j + 1];
      std::unique_ptr<IPAddress> addr = IPAddress::fromString(nh);
      if (!addr) {
        try {
          auto net = IPNetwork::fromString(nh);
          if (net)
            addr = net->toAddress();
        } catch (...) {
          // ignore parse error, leave addr null
        }
      }
      nexthop = std::move(addr);
      j += 2;
      continue;
    }
    if (opt == "vrf" && j + 1 < tokens.size()) {
      vrf = std::make_unique<VRFToken>(tokens[j + 1]);
      j += 2;
      continue;
    }
    if (opt == "interface" && j + 1 < tokens.size()) {
      interface = std::make_unique<InterfaceToken>(InterfaceType::Unknown,
                                                   tokens[j + 1]);
      j += 2;
      continue;
    }
    if (opt == "blackhole") {
      blackhole = true;
      ++j;
      continue;
    }
    break;
  }
  return j;
}

std::string RouteToken::renderTable(ConfigurationManager *mgr) const {
  if (!mgr)
    return "Error: No configuration manager\n";

  std::vector<ConfigData> routes;
  if (prefix_.empty()) {
    // Get all routes
    routes = mgr->getRoutes();
  } else {
    // Get specific route - for now, get all and filter
    auto allRoutes = mgr->getRoutes();
    for (auto &r : allRoutes) {
      if (r.route && r.route->prefix == prefix_) {
        routes.push_back(std::move(r));
        break;
      }
    }
  }

  if (routes.empty()) {
    return "No routes found.\n";
  }

  // Format as table
  std::ostringstream oss;
  oss << std::left;
  oss << std::setw(20) << "Destination" << std::setw(18) << "Gateway"
      << std::setw(12) << "Interface" << std::setw(10) << "VRF" << "Flags\n";
  oss << std::string(70, '-') << "\n";

  for (const auto &cd : routes) {
    if (!cd.route)
      continue;
    const auto &route = *cd.route;

    std::string dest = route.prefix.empty() ? "-" : route.prefix;
    std::string gateway = route.nexthop.value_or("-");
    std::string iface = route.iface.value_or("-");
    std::string vrf = route.vrf.value_or("-");
    std::string flags;
    if (route.blackhole)
      flags += "B";
    if (!route.nexthop)
      flags += "C"; // Connected
    else
      flags += "UG"; // Up, Gateway

    oss << std::setw(20) << dest << std::setw(18) << gateway << std::setw(12)
        << iface << std::setw(10) << vrf << flags << "\n";
  }

  return oss.str();
}
