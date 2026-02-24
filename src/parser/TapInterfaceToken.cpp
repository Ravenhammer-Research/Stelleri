/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 */

#include "ConfigurationManager.hpp"
#include "InterfaceToken.hpp"
#include "TapInterfaceConfig.hpp"
#include "TapTableFormatter.hpp"
#include <iostream>

class TapInterfaceToken : public InterfaceToken {
public:
  using InterfaceToken::InterfaceToken;
};

std::string InterfaceToken::toString(TapInterfaceConfig *cfg) {
  if (!cfg)
    return std::string();
  return InterfaceToken::toString(static_cast<InterfaceConfig *>(cfg));
}

bool InterfaceToken::parseTapKeywords(std::shared_ptr<InterfaceToken> &tok
                                      [[maybe_unused]],
                                      const std::vector<std::string> &tokens
                                      [[maybe_unused]],
                                      size_t &cur [[maybe_unused]]) {
  return false;
}

std::vector<std::string> InterfaceToken::tapCompletions(const std::string &prev
                                                        [[maybe_unused]]) {
  return {};
}

void InterfaceToken::setTapInterface(const InterfaceToken &tok,
                                     ConfigurationManager *mgr,
                                     InterfaceConfig &base, bool exists) {
  TapInterfaceConfig tc(base);
  tc.save(*mgr);
  std::cout << "set interface: " << (exists ? "updated" : "created") << " tap '"
            << tok.name() << "'\n";
}

std::string
InterfaceToken::showTapInterfaces(const std::vector<InterfaceConfig> &ifaces,
                                  ConfigurationManager *) {
  TapTableFormatter f;
  return f.format(ifaces);
}
