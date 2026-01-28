#pragma once

#include "ConfigurationManager.hpp"
#include "InterfaceType.hpp"
#include "Token.hpp"
#include <iostream>
#include <optional>
#include <string>

class InterfaceToken : public Token {
public:
  InterfaceToken(InterfaceType t, std::string name);

  std::string toString() const override;
  std::vector<std::string> autoComplete(std::string_view) const override;
  std::unique_ptr<Token> clone() const override;

  const std::string &name() const { return name_; }
  InterfaceType type() const { return type_; }
  std::optional<std::string> vrf;
  std::optional<std::string> source_interface;

  // Render interfaces as a table. If name is empty, get all interfaces from
  // manager; otherwise get the specific interface by name. Returns formatted
  // table string.
  std::string renderTable(ConfigurationManager *mgr) const;

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
