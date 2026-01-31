/**
 * @file Tokenizer.hpp
 * @brief Command input tokenization
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>

/**
 * @brief netcli tokenization namespace
 */
namespace netcli {
  /**
   * @brief Split command input into individual tokens
   * @param input Raw command string from user
   * @return Vector of tokens (whitespace-separated, respects quotes)
   */
  std::vector<std::string> tokenize(std::string_view input);
}
