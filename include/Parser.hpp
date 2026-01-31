/**
 * @file Parser.hpp
 * @brief Command parser for netcli commands
 */

#pragma once

#include "Command.hpp"
#include <string>
#include <vector>

/**
 * @brief netcli command parsing namespace
 */
namespace netcli {
  /**
   * @brief Parse tokenized command input into a Command object
   * @param tokens Vector of command tokens (from tokenizer)
   * @return Unique pointer to Command with parsed tokens
   */
  std::unique_ptr<Command> parse(const std::vector<std::string> &tokens);
} // namespace netcli
