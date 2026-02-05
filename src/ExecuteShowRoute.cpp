#include "ConfigurationManager.hpp"
#include "Parser.hpp"
#include "RouteTableFormatter.hpp"
#include "RouteToken.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

void netcli::Parser::executeShowRoute(const RouteToken &tok,
                                      ConfigurationManager *mgr) const {
  if (!mgr) {
    std::cout << "No ConfigurationManager provided\n";
    return;
  }
  std::vector<ConfigData> routes;
  if (tok.prefix().empty()) {
    routes = mgr->getRoutes();
  } else {
    auto allRoutes = mgr->getRoutes();
    for (auto &r : allRoutes) {
      if (r.route && r.route->prefix == tok.prefix()) {
        routes.push_back(std::move(r));
        break;
      }
    }
  }

  if (routes.empty()) {
    std::cout << "No routes found.\n";
    return;
  }

  std::string vrfContext = "Global";
  if (!routes.empty() && routes[0].route && routes[0].route->vrf) {
    vrfContext = *routes[0].route->vrf;
  }

  RouteTableFormatter formatter;
  std::cout << formatter.format(routes);
}
