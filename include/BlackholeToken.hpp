/**
 * @file BlackholeToken.hpp
 * @brief Token for blackhole route flag
 */

#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

/**
 * @brief Token representing a blackhole route
 *
 * Routes with blackhole flag silently drop traffic without sending ICMP.
 */
class BlackholeToken : public Token {
public:
  BlackholeToken() = default;

  std::string toString() const override { return "blackhole"; }

  std::vector<std::string> autoComplete(std::string_view) const override {
    return {};
  }

  std::unique_ptr<Token> clone() const override {
    return std::make_unique<BlackholeToken>(*this);
  }
};
