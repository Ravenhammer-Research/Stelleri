#include "Parser.hpp"
#include "RouteToken.hpp"
#include "ConfigurationManager.hpp"
#include <iostream>

void netcli::Parser::executeDeleteRoute(const RouteToken &tok, ConfigurationManager *mgr) const {
  (void)tok;
  (void)mgr;
  // Stub: route deletion to be implemented
  std::cout << "delete route: (stub)\n";
}
