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

  /** @brief textual reconstruction removed */

  /** @brief Get autocomplete suggestions (none for delete command) */
  std::vector<std::string> autoComplete(std::string_view) const override;

  /** @brief Clone the token */
  std::unique_ptr<Token> clone() const override;
};
