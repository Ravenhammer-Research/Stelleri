/* CommitToken.hpp
 * Token representing `set commit` commands (NETCONF commit RPC)
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Token.hpp"
#include <memory>

class CommitToken : public Token {
public:
  CommitToken() = default;
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
    return std::make_unique<CommitToken>(*this);
  }

  void executeSet(ConfigurationManager *mgr) const;

  static std::shared_ptr<CommitToken>
  parseFromTokens(const std::vector<std::string> &tokens, size_t start,
                  size_t &next);
};
