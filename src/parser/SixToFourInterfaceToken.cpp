/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 */

#include "ConfigurationManager.hpp"
#include "InterfaceToken.hpp"
#include "SixToFourInterfaceConfig.hpp"
#include "SixToFourTableFormatter.hpp"
#include <iostream>

class SixToFourInterfaceToken : public InterfaceToken {
public:
  using InterfaceToken::InterfaceToken;
};

std::string InterfaceToken::toString(SixToFourInterfaceConfig *cfg) {
  if (!cfg)
    return std::string();
  return InterfaceToken::toString(static_cast<InterfaceConfig *>(cfg));
}

bool InterfaceToken::parseSixToFourKeywords(
    std::shared_ptr<InterfaceToken> &tok [[maybe_unused]],
    const std::vector<std::string> &tokens [[maybe_unused]],
    size_t &cur [[maybe_unused]]) {
  return false;
}

std::vector<std::string>
InterfaceToken::sixToFourCompletions(const std::string &prev [[maybe_unused]]) {
  return {};
}

void InterfaceToken::setSixToFourInterface(const InterfaceToken &tok,
                                           ConfigurationManager *mgr,
                                           InterfaceConfig &base, bool exists) {
  SixToFourInterfaceConfig sc(base);
  sc.save(*mgr);
  std::cout << "set interface: " << (exists ? "updated" : "created") << " stf '"
            << tok.name() << "'\n";
}

std::string InterfaceToken::showSixToFourInterfaces(
    const std::vector<InterfaceConfig> &ifaces, ConfigurationManager *) {
  SixToFourTableFormatter f;
  return f.format(ifaces);
}
