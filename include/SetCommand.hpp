/**
 * @file SetCommand.hpp
 * @brief Set command token
 */

#pragma once

#include "Token.hpp"
#include <string>

/**
 * @brief Token representing the "set" command
 * 
 * Used to create or modify configuration entries (interfaces, routes, VRFs, etc.).
 */
class SetCommand : public Token {
public:
  SetCommand() = default;
  
  /** @brief Get string representation of the token */
  std::string toString() const override { return "set"; }
  
  /** @brief Get autocomplete suggestions for configuration types */
  std::vector<std::string> autoComplete(std::string_view) const override {
    return {"interfaces", "vrf", "protocols"};
  }
  
  /** @brief Clone the token */
  std::unique_ptr<Token> clone() const override {
    return std::make_unique<SetCommand>(*this);
  }
};
