#include "StaticToken.hpp"

std::vector<std::string> StaticToken::autoComplete(std::string_view) const {
  return {};
}

std::unique_ptr<Token> StaticToken::clone() const {
  return std::make_unique<StaticToken>(*this);
}
#include "StaticToken.hpp"

// Implementation in header
