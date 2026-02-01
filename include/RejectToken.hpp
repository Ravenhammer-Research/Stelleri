/**
 * @file RejectToken.hpp
 * @brief Token for reject route flag
 */

#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

/**
 * @brief Token representing a reject route
 *
 * Routes with reject flag actively refuse traffic and send ICMP unreachable.
 */
class RejectToken : public Token {
public:
  RejectToken() = default;

  // textual reconstruction removed

  std::vector<std::string> autoComplete(std::string_view) const override;

  std::unique_ptr<Token> clone() const override;
};
