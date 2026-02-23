/* TargetToken.hpp
 * Token representing `set target ...` commands for NETCONF client targeting
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Token.hpp"
#include <optional>
#include <string>

class TargetToken : public Token {
public:
  enum class Type { Unix, Ssh, Tls, Unknown };

  TargetToken() = default;
  std::vector<std::string>
  autoComplete(std::string_view partial) const override {
    (void)partial;
    return {};
  }
  std::vector<std::string>
  autoComplete(const std::vector<std::string> &tokens, std::string_view partial,
               ConfigurationManager *mgr) const override {
    (void)tokens;
    (void)partial;
    (void)mgr;
    return {};
  }
  std::unique_ptr<Token> clone() const override {
    return std::make_unique<TargetToken>(*this);
  }

  void executeSet(ConfigurationManager *mgr) const;

  // Parse tokens starting at `start` (pointing to "target") and set `next`
  // to the index after consumed tokens. Returns nullptr on parse error.
  static std::shared_ptr<TargetToken>
  parseFromTokens(const std::vector<std::string> &tokens, size_t start,
                  size_t &next);

  Type type = Type::Unknown;
  std::string path;
  std::string host;
  std::optional<uint16_t> port;
};
