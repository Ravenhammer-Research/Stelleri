/**
 * @file IPv6Address.hpp
 * @brief IPv6 address implementation
 */

#pragma once

#include "IPAddress.hpp"
#include <cstdint>
#include <memory>
#include <string>

/**
 * @brief IPv6 address (128-bit)
 * 
 * Stores IPv6 addresses as 128-bit unsigned integers.
 * Supports standard colon-hexadecimal notation with compression (e.g., "2001:db8::1").
 */
class IPv6Address : public IPAddress {
public:
  /**
   * @brief Construct from 128-bit unsigned integer
   * @param v IPv6 address as 128-bit value
   */
  explicit IPv6Address(unsigned __int128 v) : IPAddress(v) {}
  
  /** @brief Get address family (always IPv6) */
  AddressFamily family() const override { return AddressFamily::IPv6; }
  
  /** @brief Get address as 128-bit unsigned integer */
  unsigned __int128 asUint128() const { return value(); }
  
  /** @brief Get colon-hexadecimal string representation (e.g., "2001:db8::1") */
  std::string toString() const override;
  
  /** @brief Clone the address object */
  std::unique_ptr<IPAddress> clone() const override;
};
