#pragma once

#include "Token.hpp"
#include <string>

class SetCommand : public Token {
public:
  SetCommand() = default;
  std::string toString() const override { return "set"; }
  std::vector<std::string> autoComplete(std::string_view) const override {
    return {"interfaces", "vrf", "protocols"};
  }
  std::unique_ptr<Token> clone() const override {
    return std::make_unique<SetCommand>(*this);
  }
};
