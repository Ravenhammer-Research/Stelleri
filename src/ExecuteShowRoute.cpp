#include "Parser.hpp"
#include "RouteToken.hpp"
#include "ConfigurationManager.hpp"
#include <iostream>

void netcli::Parser::executeShowRoute(const RouteToken &tok, ConfigurationManager *mgr) const {
  if (!mgr) {
    std::cout << "No ConfigurationManager provided\n";
    return;
  }
  std::cout << tok.renderTable(mgr);
}
