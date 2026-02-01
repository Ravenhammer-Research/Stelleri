#include "VRFToken.hpp"
#include "ConfigurationManager.hpp"
#include <iostream>

void executeSetVRF(const VRFToken &tok, ConfigurationManager *mgr) {
  (void)tok;
  (void)mgr;
  // Stub: creating VRF (FIB) will be implemented later
  std::cout << "set vrf: (stub)\n";
}
