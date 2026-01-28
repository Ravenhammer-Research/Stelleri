#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

class ProtocolsToken : public Token {
public:
  explicit ProtocolsToken(std::string proto) : proto_(std::move(proto)) {}
  std::string toString() const override {
    return std::string("protocols ") + proto_;
  }
  std::vector<std::string> autoComplete(std::string_view) const override {
    return {"static"};
  }
  std::unique_ptr<Token> clone() const override {
    return std::make_unique<ProtocolsToken>(*this);
  }
  const std::string &proto() const { return proto_; }

private:
  std::string proto_;
};
