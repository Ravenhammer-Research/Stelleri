#pragma once

#include <cstdint>
#include <memory>
#include <string>
// Forward declaration to avoid circular include with IPNetwork

enum class AddressFamily { IPv4, IPv6 };

class IPAddress {
protected:
  unsigned __int128 value_ = 0;
  uint8_t mask_ = 0;

public:
  IPAddress() = default;
  explicit IPAddress(unsigned __int128 v) : value_(v) {}
  IPAddress(unsigned __int128 v, uint8_t mask) : value_(v), mask_(mask) {}
  virtual ~IPAddress() = default;

  unsigned __int128 value() const { return value_; }

  uint8_t mask() const { return mask_; }

  // Return family (IPv4 / IPv6)
  virtual AddressFamily family() const = 0;

  // (removed) Create a network object with the provided mask

  // Return textual representation of the address
  virtual std::string toString() const = 0;

  // Clone the address object
  virtual std::unique_ptr<IPAddress> clone() const = 0;

  // Parse an address string (IPv4 or IPv6 textual form) and return an IPAddress
  static std::unique_ptr<IPAddress> fromString(const std::string &s);
};
