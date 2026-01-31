/**
 * @file IPv4Address.hpp
 * @brief IPv4 address implementation
 */

#pragma once

#include "IPAddress.hpp"
#include <cstdint>
#include <memory>
#include <string>

/**
 * @brief IPv4 address (32-bit)
 * 
 * Stores IPv4 addresses as 32-bit unsigned integers.
 * Supports standard dotted-decimal notation (e.g., "192.168.1.1").
 */
class IPv4Address : public IPAddress {
public:
  /**
   * @brief Construct from 32-bit unsigned integer
   * @param v IPv4 address as 32-bit value (network byte order)
   */
  explicit IPv4Address(uint32_t v)
      : IPAddress(static_cast<unsigned __int128>(v)) {}
  
  /**
   * @brief Construct from 128-bit unsigned integer
   * @param v IPv4 address stored in 128-bit value
   */
  explicit IPv4Address(unsigned __int128 v) : IPAddress(v) {}
  
  /** @brief Get address family (always IPv4) */
  AddressFamily family() const override { return AddressFamily::IPv4; }
  
  /** @brief Get address as 32-bit unsigned integer */
  uint32_t asUint32() const { return static_cast<uint32_t>(value()); }
  
  /** @brief Get dotted-decimal string representation (e.g., "192.168.1.1") */
  std::string toString() const override;
  
  /** @brief Clone the address object */
  std::unique_ptr<IPAddress> clone() const override;
};
