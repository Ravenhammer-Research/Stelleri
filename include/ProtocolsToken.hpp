/**
 * @file ProtocolsToken.hpp
 * @brief Routing protocols token
 */

#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

/**
 * @brief Token representing routing protocols
 *
 * Used in route configuration to specify protocol type (e.g., "static").
 * Example: "set route protocols static 0.0.0.0/0 next-hop 192.168.1.1"
 */
class ProtocolsToken : public Token {
public:
  /**
   * @brief Construct with protocol name
   * @param proto Protocol name (e.g., "static")
   */
  explicit ProtocolsToken(std::string proto);

  /** @brief textual reconstruction removed */

  /** @brief Get autocomplete suggestions (currently only "static") */
  std::vector<std::string> autoComplete(std::string_view) const override;

  /** @brief Clone the token */
  std::unique_ptr<Token> clone() const override;

  /** @brief Get protocol name */
  const std::string &proto() const;

private:
  std::string proto_;
};
