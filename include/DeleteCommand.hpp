#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

class DeleteCommand : public Token {
public:
  DeleteCommand() = default;
  std::string toString() const override { return "delete"; }
  std::vector<std::string> autoComplete(std::string_view) const override {
    return {};
  }
  std::unique_ptr<Token> clone() const override {
    return std::make_unique<DeleteCommand>(*this);
  }
};
