#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

class ShowCommand : public Token {
public:
  ShowCommand() = default;
  std::string toString() const override { return "show"; }
  std::vector<std::string> autoComplete(std::string_view) const override {
    return {"interfaces", "vrf", "protocols", "route"};
  }
  std::unique_ptr<Token> clone() const override {
    return std::make_unique<ShowCommand>(*this);
  }
};
