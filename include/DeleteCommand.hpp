/**
 * @file DeleteCommand.hpp
 * @brief Delete command token
 */

#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

/**
 * @brief Token representing the "delete" command
 * 
 * Used to delete configuration entries (interfaces, routes, VRFs, etc.).
 */
class DeleteCommand : public Token {
public:
  DeleteCommand() = default;
  
  /** @brief Get string representation of the token */
  std::string toString() const override { return "delete"; }
  
  /** @brief Get autocomplete suggestions (none for delete command) */
  std::vector<std::string> autoComplete(std::string_view) const override {
    return {};
  }
  
  /** @brief Clone the token */
  std::unique_ptr<Token> clone() const override {
    return std::make_unique<DeleteCommand>(*this);
  }
};
