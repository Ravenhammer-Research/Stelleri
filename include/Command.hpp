/**
 * @file Command.hpp
 * @brief Command container and builder
 */

#pragma once

#include "Token.hpp"
#include <memory>
#include <sstream>
#include <string>

/**
 * @brief Container for a parsed command with linked token chain
 * 
 * Builds and manages a chain of tokens representing a complete command.
 */
class Command {
public:
  Command() = default;
  ~Command() = default;

  /**
   * @brief Add a token to the command chain
   * @param t Token to add
   */
  void addToken(std::shared_ptr<Token> t) {
    if (!head_) {
      head_ = t;
      tail_ = t;
      return;
    }
    tail_->setNext(t);
    tail_ = t;
  }

  // Reconstruct the full CLI command by joining each Token's CLI fragment
  std::string toString() const {
    std::ostringstream oss;
    auto cur = head_;
    bool first = true;
    while (cur) {
      if (!first)
        oss << ' ';
      oss << cur->toString();
      first = false;
      cur = cur->getNext();
    }
    return oss.str();
  }

  // Basic validation stub; detailed grammar validation via Parser
  bool validate() const { return static_cast<bool>(head_); }

  std::shared_ptr<Token> head() const { return head_; }

private:
  std::shared_ptr<Token> head_;
  std::shared_ptr<Token> tail_;
};
