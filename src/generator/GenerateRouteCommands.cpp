/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 */

#include "CommandGenerator.hpp"
#include "RouteConfig.hpp"
#include "RouteToken.hpp"
#include <iostream>

namespace netcli {

  void CommandGenerator::generateRoutes(ConfigurationManager &mgr) {
    auto routes = mgr.GetRoutes();
    for (const auto &r : routes) {
      // Only emit manually-configured (static) routes.  Connected-subnet,
      // loopback, link-local, and multicast routes are auto-created when
      // interfaces are brought up and must not appear in the generated config.
      if (!(r.flags & RouteConfig::RTF_STATIC_FLAG))
        continue;

      // Scoped link-local /128 routes (fe80::xxx/128 scope <iface>) are
      // auto-generated when an interface gets a link-local address.
      if (r.scope)
        continue;

      std::cout << std::string("set ") +
                       RouteToken::toString(const_cast<RouteConfig *>(&r))
                << "\n";
    }
  }

} // namespace netcli
