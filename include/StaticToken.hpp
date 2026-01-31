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
  
  std::string toString() const override { return "static"; }
  
  std::vector<std::string> autoComplete(std::string_view) const override {
    return {};  // No sub-options for static protocol
  }
  
  std::unique_ptr<Token> clone() const override {
    return std::make_unique<StaticToken>(*this);
  }
};
