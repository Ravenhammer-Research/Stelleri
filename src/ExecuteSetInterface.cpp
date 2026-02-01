#include "Parser.hpp"
#include "InterfaceToken.hpp"
#include "ConfigurationManager.hpp"
#include <iostream>

void netcli::Parser::executeSetInterface(const InterfaceToken &tok, ConfigurationManager *mgr) const {
  (void)tok;
  (void)mgr;
  // Stub: detailed set handling will be implemented later.
  std::cout << "set interface: (stub)\n";
}
