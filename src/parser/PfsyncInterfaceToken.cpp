/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 */

#include "ConfigurationManager.hpp"
#include "InterfaceToken.hpp"
#include "PfsyncInterfaceConfig.hpp"
#include "PfsyncTableFormatter.hpp"
#include <iostream>

class PfsyncInterfaceToken : public InterfaceToken {
public:
  using InterfaceToken::InterfaceToken;
};

bool InterfaceToken::parsePfsyncKeywords(std::shared_ptr<InterfaceToken> &tok
                                         [[maybe_unused]],
                                         const std::vector<std::string> &tokens
                                         [[maybe_unused]],
                                         size_t &cur [[maybe_unused]]) {
  return false;
}

std::vector<std::string>
InterfaceToken::pfsyncCompletions(const std::string &prev [[maybe_unused]]) {
  return {};
}

void InterfaceToken::setPfsyncInterface(const InterfaceToken &tok,
                                        ConfigurationManager *mgr,
                                        InterfaceConfig &base, bool exists) {
  PfsyncInterfaceConfig pc(base);
  pc.save(*mgr);
  std::cout << "set interface: " << (exists ? "updated" : "created")
            << " pfsync '" << tok.name() << "'\n";
}

std::string
InterfaceToken::showPfsyncInterfaces(const std::vector<InterfaceConfig> &ifaces,
                                     ConfigurationManager *) {
  PfsyncTableFormatter f;
  return f.format(ifaces);
}
