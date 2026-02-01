/**
 * @file VRFToken.hpp
 * @brief Virtual Routing and Forwarding (VRF) token
 */

#pragma once

#include "Token.hpp"
#include <optional>
#include <string>

/**
 * @brief Token representing a VRF (Virtual Routing and Forwarding) instance
 *
 * VRFs provide routing table isolation. Each VRF has a name and optional
 * routing table ID.
 */
class VRFToken : public Token {
public:
  /**
   * @brief Construct VRF token with name
   * @param name VRF instance name
   */
  explicit VRFToken(std::string name);

  /** @brief textual reconstruction removed */

  /** @brief Get autocomplete suggestions (none for VRF) */
  std::vector<std::string> autoComplete(std::string_view) const override;

  /** @brief Clone the token */
  std::unique_ptr<Token> clone() const override;

  /** @brief Get VRF name */
  const std::string &name() const { return name_; }

  /** @brief Optional routing table ID for FreeBSD setfib() */
  std::optional<int> table;

private:
  std::string name_;
};
