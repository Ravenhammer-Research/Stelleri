#include "Parser.hpp"
#include "RouteToken.hpp"
#include "ConfigurationManager.hpp"
#include "RouteConfig.hpp"
#include <iostream>

void netcli::Parser::executeSetRoute(const RouteToken &tok, ConfigurationManager *mgr) const {
  (void)mgr;
  RouteConfig rc;
  rc.prefix = tok.prefix();
  if (tok.nexthop) rc.nexthop = tok.nexthop->toString();
  if (tok.interface) rc.iface = tok.interface->name();
  if (tok.vrf) rc.vrf = tok.vrf->name();
  rc.blackhole = tok.blackhole;
  rc.reject = tok.reject;
  rc.save();
  std::cout << "set route: " << rc.prefix << " (stub saved)\n";
}
