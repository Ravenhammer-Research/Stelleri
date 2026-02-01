#include "Parser.hpp"
#include "InterfaceToken.hpp"
#include "ConfigurationManager.hpp"
#include <iostream>

void netcli::Parser::executeDeleteInterface(const InterfaceToken &tok, ConfigurationManager *mgr) const {
  (void)tok;
  (void)mgr;
  // Stub: delete via InterfaceConfig::destroy() will be integrated later.
  std::cout << "delete interface: (stub)\n";
}
