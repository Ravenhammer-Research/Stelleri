#include "ConfigurationManager.hpp"
#include "Parser.hpp"
#include "VRFToken.hpp"
#include <iostream>

void netcli::Parser::executeShowVRF(const VRFToken &tok,
                                    ConfigurationManager *mgr) const {
  (void)mgr;
  std::cout << "VRF: " << tok.name() << "\n";
}
