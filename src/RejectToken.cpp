#include "RejectToken.hpp"

std::vector<std::string> RejectToken::autoComplete(std::string_view) const {
  return {};
}

std::unique_ptr<Token> RejectToken::clone() const {
  return std::make_unique<RejectToken>(*this);
}
