/**
 * @file DeleteToken.hpp
 * @brief Token representing the "delete" verb
 */

#pragma once

#include "Token.hpp"
#include <memory>
#include <string>
#include <vector>

class DeleteToken : public Token {
public:
  DeleteToken() = default;
  ~DeleteToken() override = default;

  std::vector<std::string> autoComplete(std::string_view) const override;

  std::unique_ptr<Token> clone() const override;
};
