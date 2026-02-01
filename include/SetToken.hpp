/**
 * @file SetToken.hpp
 * @brief Token representing the "set" verb
 */

#pragma once

#include "Token.hpp"
#include <memory>
#include <string>
#include <vector>

class SetToken : public Token {
public:
  SetToken() = default;
  ~SetToken() override = default;

  std::vector<std::string> autoComplete(std::string_view) const override;

  std::unique_ptr<Token> clone() const override;
};
