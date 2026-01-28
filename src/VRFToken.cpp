#include "VRFToken.hpp"

VRFToken::VRFToken(std::string name) : name_(std::move(name)) {}

std::string VRFToken::toString() const {
  // prefer simple quoted form
  return std::string("vrf '") + name_ + "'";
}

std::vector<std::string> VRFToken::autoComplete(std::string_view) const {
  return {};
}

std::unique_ptr<Token> VRFToken::clone() const {
  return std::make_unique<VRFToken>(*this);
}
