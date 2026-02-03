#include "BlackholeToken.hpp"

std::vector<std::string> BlackholeToken::autoComplete(std::string_view) const {
  return {};
}

std::unique_ptr<Token> BlackholeToken::clone() const {
  return std::make_unique<BlackholeToken>(*this);
}
