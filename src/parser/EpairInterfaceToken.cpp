/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 */

#include "ConfigurationManager.hpp"
#include "EpairInterfaceConfig.hpp"
#include "EpairTableFormatter.hpp"
#include "InterfaceToken.hpp"
#include "SingleEpairSummaryFormatter.hpp"
#include <iostream>

class EpairInterfaceToken : public InterfaceToken {
public:
  using InterfaceToken::InterfaceToken;
};

bool InterfaceToken::parseEpairKeywords(std::shared_ptr<InterfaceToken> &tok
                                        [[maybe_unused]],
                                        const std::vector<std::string> &tokens
                                        [[maybe_unused]],
                                        size_t &cur [[maybe_unused]]) {
  return false;
}

std::vector<std::string>
InterfaceToken::epairCompletions(const std::string &prev [[maybe_unused]]) {
  return {};
}

void InterfaceToken::setEpairInterface(const InterfaceToken &tok,
                                       ConfigurationManager *mgr,
                                       InterfaceConfig &base, bool exists) {
  EpairInterfaceConfig vic(base);
  vic.save(*mgr);
  std::cout << "set interface: " << (exists ? "updated" : "created")
            << " epair '" << tok.name() << "'\n";
}

bool InterfaceToken::showEpairInterface(const InterfaceConfig &ic,
                                        ConfigurationManager *mgr) {
  std::vector<InterfaceConfig> v = {ic};
  auto epairs = mgr->GetEpairInterfaces(v);
  if (!epairs.empty()) {
    SingleEpairSummaryFormatter f;
    std::cout << f.format(epairs[0]);
    return true;
  }
  return false;
}

std::string
InterfaceToken::showEpairInterfaces(const std::vector<InterfaceConfig> &ifaces,
                                    ConfigurationManager *) {
  EpairTableFormatter f;
  return f.format(ifaces);
}
