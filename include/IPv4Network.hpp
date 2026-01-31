/**
 * @file IPv4Network.hpp
 * @brief IPv4 network with CIDR prefix
 */

#pragma once

#include "IPNetwork.hpp"
#include "IPv4Address.hpp"
#include <cstdint>
#include <memory>

/**
 * @brief IPv4 network (address + prefix length)
 * 
 * Represents IPv4 networks in CIDR notation (e.g., "192.168.1.0/24").
 */
class IPv4Network : public IPNetwork {
public:
  /**
   * @brief Construct from 32-bit address and prefix length
   * @param v IPv4 network address as 32-bit value
   * @param mask Prefix length (0-32)
   */
  IPv4Network(uint32_t v, uint8_t mask)
      : IPNetwork(static_cast<unsigned __int128>(v), mask) {}
  
  /**
   * @brief Construct from 128-bit address and prefix length
   * @param v IPv4 network address in 128-bit storage
   * @param mask Prefix length (0-32)
   */
  IPv4Network(unsigned __int128 v, uint8_t mask) : IPNetwork(v, mask) {}
  
  /** @brief Get address family (always IPv4) */
  AddressFamily family() const override { return AddressFamily::IPv4; }
  
  /** @brief Convert network to address object */
  std::unique_ptr<IPAddress> toAddress() const override;
  
  /** @brief Clone the network object */
  std::unique_ptr<IPNetwork> cloneNetwork() const override;
  
  /** @brief Get CIDR string representation (e.g., "192.168.1.0/24") */
  std::string toString() const override;
  
  /** @brief Clone as address object */
  std::unique_ptr<IPAddress> clone() const override;
  
  /** @brief Get network address as 32-bit unsigned integer */
  uint32_t networkUint32() const { return static_cast<uint32_t>(value()); }
};
