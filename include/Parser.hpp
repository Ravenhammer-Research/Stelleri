#pragma once

#include "Command.hpp"
#include <string>
#include <vector>

namespace netcli {
  // Very small parser API: produce an empty Command for now or a Command with
  // tokens
  std::unique_ptr<Command> parse(const std::vector<std::string> &tokens);
} // namespace netcli
