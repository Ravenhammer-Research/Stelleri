#include "VRFToken.hpp"
#include "ConfigurationManager.hpp"
#include <iostream>

void executeShowVRF(const VRFToken &tok, ConfigurationManager *mgr) {
  (void)mgr;
  std::cout << "VRF: " << tok.name() << "\n";
}
