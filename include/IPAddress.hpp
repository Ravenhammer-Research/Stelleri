/**
 * @file IPAddress.hpp
 * @brief IP address base class
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>

/**
 * @brief IP address family (IPv4 or IPv6)
 */
enum class AddressFamily { IPv4, IPv6 };

/**
 * @brief Base class for IP addresses
 * 
 * Uses 128-bit storage to handle both IPv4 and IPv6 addresses uniformly.
 * Derived classes implement family-specific behavior.
 */
class IPAddress {
protected:
  unsigned __int128 value_ = 0;
  uint8_t mask_ = 0;

public:
  IPAddress() = default;
  explicit IPAddress(unsigned __int128 v) : value_(v) {}
  IPAddress(unsigned __int128 v, uint8_t mask) : value_(v), mask_(mask) {}
  virtual ~IPAddress() = default;

  /** @brief Get raw 128-bit address value */
  unsigned __int128 value() const { return value_; }

  /** @brief Get prefix length/netmask */
  uint8_t mask() const { return mask_; }

  /** @brief Get address family (IPv4 or IPv6) */
  virtual AddressFamily family() const = 0;

  /** @brief Get textual representation of the address */
  virtual std::string toString() const = 0;

  /** @brief Clone the address object */
  virtual std::unique_ptr<IPAddress> clone() const = 0;

  /**
   * @brief Parse an address string (IPv4 or IPv6 textual form)
   * @param s IP address string
   * @return Unique pointer to IPAddress, or nullptr if invalid
   */
  static std::unique_ptr<IPAddress> fromString(const std::string &s);
};
