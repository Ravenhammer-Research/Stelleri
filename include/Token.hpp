#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

class Token : public std::enable_shared_from_this<Token> {
public:
  virtual ~Token() = default;
  // CLI fragment representation used to reconstruct commands
  // Example: InterfaceToken::toString() -> "interfaces ethernet eth1"
  virtual std::string toString() const = 0;
  // Provide completion suggestions for partial input
  virtual std::vector<std::string>
  autoComplete(std::string_view partial) const = 0;
  // Clone token (for copy/transform operations)
  virtual std::unique_ptr<Token> clone() const = 0;

  void setNext(std::shared_ptr<Token> n) { next_ = std::move(n); }
  std::shared_ptr<Token> getNext() const { return next_; }
  bool hasNext() const { return static_cast<bool>(next_); }

protected:
  std::shared_ptr<Token> next_;
};
