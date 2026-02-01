#include "DeleteCommand.hpp"

std::vector<std::string> DeleteCommand::autoComplete(std::string_view) const {
  return {};
}

std::unique_ptr<Token> DeleteCommand::clone() const {
  return std::make_unique<DeleteCommand>(*this);
}
