#include "DeleteToken.hpp"

std::vector<std::string> DeleteToken::autoComplete(std::string_view) const {
  return {"interfaces", "route", "vrf"};
}

std::unique_ptr<Token> DeleteToken::clone() const {
  return std::make_unique<DeleteToken>(*this);
}
