#include "ConfigurationManager.hpp"
#include "Parser.hpp"
#include "VRFToken.hpp"
#include <iostream>

void netcli::Parser::executeDeleteVRF(const VRFToken &tok,
                                      ConfigurationManager *mgr) const {
  (void)tok;
  (void)mgr;
  // Stub: VRF deletion will be implemented later
  std::cout << "delete vrf: (stub)\n";
}
