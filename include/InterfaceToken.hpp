#pragma once

#include "BridgeInterfaceConfig.hpp"
#include "ConfigurationManager.hpp"
#include "InterfaceType.hpp"
#include "LaggConfig.hpp"
#include "Token.hpp"
#include "TunnelConfig.hpp"
#include "VLANConfig.hpp"
#include <iostream>
#include <optional>
#include <string>

class InterfaceToken : public Token {
public:
  InterfaceToken(InterfaceType t, std::string name);

  // textual reconstruction removed
  std::vector<std::string> autoComplete(std::string_view) const override;
  std::unique_ptr<Token> clone() const override;

  const std::string &name() const { return name_; }
  InterfaceType type() const { return type_; }
  std::optional<std::string> vrf;
  std::optional<std::string> group;
  std::optional<int> tunnel_vrf;
  std::optional<std::string> address;
  std::optional<int> address_family; // AF_INET or AF_INET6 when inet/inet6 used
  std::optional<int> mtu;
  std::optional<BridgeInterfaceConfig> bridge;
  std::optional<LaggConfig> lagg;
  std::optional<VLANConfig> vlan;
  std::optional<TunnelConfig> tunnel;

  // (Rendering moved to execute handlers. Token is parse-only.)

  // Parse an `interfaces` sequence starting at index `start` in `tokens`.
  // `start` should point to the "interfaces" token. On success, returns a
  // shared_ptr to an InterfaceToken and sets `next` to the index after consumed
  // tokens. Recognized forms:
  //  - interfaces
  //  - interfaces <type> <name>   where <type> is ethernet|loopback|pppoe
  static std::shared_ptr<InterfaceToken>
  parseFromTokens(const std::vector<std::string> &tokens, size_t start,
                  size_t &next);

private:
  InterfaceType type_ = InterfaceType::Unknown;
  std::string name_;
};
