#include "ShowToken.hpp"

std::vector<std::string> ShowToken::autoComplete(std::string_view) const {
  return {"interfaces", "route", "vrf"};
}

std::unique_ptr<Token> ShowToken::clone() const {
  return std::make_unique<ShowToken>(*this);
}
