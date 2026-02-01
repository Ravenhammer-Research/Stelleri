#include "ProtocolsToken.hpp"

ProtocolsToken::ProtocolsToken(std::string proto) : proto_(std::move(proto)) {}

std::vector<std::string> ProtocolsToken::autoComplete(std::string_view) const {
  return {"static"};
}

std::unique_ptr<Token> ProtocolsToken::clone() const {
  return std::make_unique<ProtocolsToken>(*this);
}

const std::string &ProtocolsToken::proto() const { return proto_; }
#include "ProtocolsToken.hpp"

// Minimal implementation; logic lives in header
