#include "ConfigurationManager.hpp"
#include "Parser.hpp"
#include "VRFToken.hpp"
#include <iostream>

void netcli::Parser::executeSetVRF(const VRFToken &tok,
                                   ConfigurationManager *mgr) const {
  (void)tok;
  (void)mgr;
  // Stub: creating VRF (FIB) will be implemented later
  std::cout << "set vrf: (stub)\n";
}
