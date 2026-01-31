/**
 * @file ShowCommand.hpp
 * @brief Show command token
 */

#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

/**
 * @brief Token representing the "show" command
 *
 * Used to display configuration data (interfaces, routes, VRFs, etc.).
 */
class ShowCommand : public Token {
public:
  ShowCommand() = default;

  /** @brief Get string representation of the token */
  std::string toString() const override { return "show"; }

  /** @brief Get autocomplete suggestions for showable objects */
  std::vector<std::string> autoComplete(std::string_view) const override {
    return {"interfaces", "vrf", "protocols", "route"};
  }

  /** @brief Clone the token */
  std::unique_ptr<Token> clone() const override {
    return std::make_unique<ShowCommand>(*this);
  }
};
