#pragma once

#include "Token.hpp"
#include <optional>
#include <string>

class VRFToken : public Token {
public:
  explicit VRFToken(std::string name);
  std::string toString() const override;
  std::vector<std::string> autoComplete(std::string_view) const override;
  std::unique_ptr<Token> clone() const override;

  std::string name() const { return name_; }
  std::optional<int> table;
  bool bind_to_all = false;

private:
  std::string name_;
};
