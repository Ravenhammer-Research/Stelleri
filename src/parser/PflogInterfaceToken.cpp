/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 */

#include "ConfigurationManager.hpp"
#include "InterfaceToken.hpp"
#include "PflogInterfaceConfig.hpp"
#include "PflogTableFormatter.hpp"
#include <iostream>

class PflogInterfaceToken : public InterfaceToken {
public:
  using InterfaceToken::InterfaceToken;
};

bool InterfaceToken::parsePflogKeywords(std::shared_ptr<InterfaceToken> &tok
                                        [[maybe_unused]],
                                        const std::vector<std::string> &tokens
                                        [[maybe_unused]],
                                        size_t &cur [[maybe_unused]]) {
  return false;
}

std::vector<std::string>
InterfaceToken::pflogCompletions(const std::string &prev [[maybe_unused]]) {
  return {};
}

void InterfaceToken::setPflogInterface(const InterfaceToken &tok,
                                       ConfigurationManager *mgr,
                                       InterfaceConfig &base, bool exists) {
  PflogInterfaceConfig pc(base);
  pc.save(*mgr);
  std::cout << "set interface: " << (exists ? "updated" : "created")
            << " pflog '" << tok.name() << "'\n";
}

std::string
InterfaceToken::showPflogInterfaces(const std::vector<InterfaceConfig> &ifaces,
                                    ConfigurationManager *) {
  PflogTableFormatter f;
  return f.format(ifaces);
}
