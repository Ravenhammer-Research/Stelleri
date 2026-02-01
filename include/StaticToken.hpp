/**
 * @file StaticToken.hpp
 * @brief Token for static routing protocol
 */

#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

/**
 * @brief Token representing static routing protocol
 *
 * Used in the command chain: set route protocols static <prefix> ...
 */
class StaticToken : public Token {
public:
  StaticToken() = default;

  // textual reconstruction removed

  std::vector<std::string> autoComplete(std::string_view) const override;

  std::unique_ptr<Token> clone() const override;
};
