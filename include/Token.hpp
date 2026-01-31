/**
 * @file Token.hpp
 * @brief Base class for command tokens
 */

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Base class for command tokens in the parser chain
 * 
 * Tokens form a linked list representing parsed command structure.
 * Each token type (interface, route, VRF, etc.) implements specific behavior.
 */
class Token : public std::enable_shared_from_this<Token> {
public:
  virtual ~Token() = default;
  
  /**
   * @brief Get CLI fragment representation
   * @return String representation used to reconstruct commands
   * @example InterfaceToken::toString() -> "interfaces ethernet eth1"
   */
  virtual std::string toString() const = 0;
  
  /**
   * @brief Provide completion suggestions for partial input
   * @param partial Partial input string to complete
   * @return Vector of possible completions
   */
  virtual std::vector<std::string>
  autoComplete(std::string_view partial) const = 0;
  
  /**
   * @brief Clone token for copy/transform operations
   * @return Unique pointer to cloned token
   */
  virtual std::unique_ptr<Token> clone() const = 0;

  /** @brief Set next token in chain */
  void setNext(std::shared_ptr<Token> n) { next_ = std::move(n); }
  
  /** @brief Get next token in chain */
  std::shared_ptr<Token> getNext() const { return next_; }
  bool hasNext() const { return static_cast<bool>(next_); }

protected:
  std::shared_ptr<Token> next_;
};
