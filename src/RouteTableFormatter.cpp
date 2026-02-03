#include "RouteTableFormatter.hpp"
#include "RouteConfig.hpp"
#include <iomanip>
#include <sstream>

std::string
RouteTableFormatter::format(const std::vector<ConfigData> &routes) const {
  if (routes.empty()) {
    return "No routes found.\n";
  }

  // Determine VRF context (first route's VRF if present)
  std::string vrfContext = "Global";
  if (!routes.empty() && routes[0].route && routes[0].route->vrf)
    vrfContext = *routes[0].route->vrf;

  std::ostringstream oss;
  oss << "Routes (FIB: " << vrfContext << ")\n\n";
  oss << std::left;
  oss << std::setw(20) << "Destination" << std::setw(18) << "Gateway"
      << std::setw(12) << "Interface" << "Flags\n";
  oss << std::string(56, '-') << "\n";

  for (const auto &cd : routes) {
    if (!cd.route)
      continue;
    const auto &route = *cd.route;

    std::string dest = route.prefix.empty() ? "-" : route.prefix;
    std::string gateway = route.nexthop.value_or("-");
    std::string iface = route.iface.value_or("-");
    std::string flags;
    if (route.blackhole)
      flags += "B";
    if (route.reject)
      flags += "R";
    if (!route.nexthop)
      flags += "C"; // Connected
    else if (!route.blackhole && !route.reject)
      flags += "UG"; // Up, Gateway

    oss << std::setw(20) << dest << std::setw(18) << gateway << std::setw(12)
        << iface << flags << "\n";
  }

  return oss.str();
}
