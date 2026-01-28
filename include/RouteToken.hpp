#pragma once

#include "ConfigurationManager.hpp"
#include "IPAddress.hpp"
#include "IPNetwork.hpp"
#include "InterfaceToken.hpp"
#include "Token.hpp"
#include "VRFToken.hpp"
#include <iomanip>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

class RouteToken : public Token {
public:
  explicit RouteToken(std::string prefix);
  std::string toString() const override;
  std::vector<std::string> autoComplete(std::string_view) const override;
  std::unique_ptr<Token> clone() const override;

  const std::string &prefix() const { return prefix_; }
  std::unique_ptr<IPAddress> nexthop;
  std::unique_ptr<InterfaceToken> interface;
  std::unique_ptr<VRFToken>
      vrf; // only the id/name is completable in this context
  bool blackhole = false;

  void debugOutput(std::ostream &os) const;

  // Parse route option tokens starting at `start` and return the index after
  // consumed tokens. Recognized options: next-hop <ip|cidr>, interface <name>,
  // vrf <name>, blackhole
  size_t parseOptions(const std::vector<std::string> &tokens, size_t start);

  // Render routes as a table. If prefix is empty, get all routes from manager;
  // otherwise get the specific route by prefix. Returns formatted table string.
  std::string renderTable(ConfigurationManager *mgr) const;

private:
  std::string prefix_;
};
