/**
 * @file ShowToken.hpp
 * @brief Token representing the "show" verb
 */

#pragma once

#include "Token.hpp"
#include <memory>
#include <string>
#include <vector>

class ShowToken : public Token {
public:
  ShowToken() = default;
  ~ShowToken() override = default;

  std::vector<std::string> autoComplete(std::string_view) const override;

  std::unique_ptr<Token> clone() const override;
};
