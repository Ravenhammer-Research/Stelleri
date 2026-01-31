/**
 * @file IPv6Network.hpp
 * @brief IPv6 network with CIDR prefix
 */

#pragma once

#include "IPNetwork.hpp"
#include "IPv6Address.hpp"
#include <cstdint>
#include <memory>

/**
 * @brief IPv6 network (address + prefix length)
 * 
 * Represents IPv6 networks in CIDR notation (e.g., "2001:db8::/64").
 */
class IPv6Network : public IPNetwork {
public:
  /**
   * @brief Construct from 128-bit address and prefix length
   * @param v IPv6 network address as 128-bit value
   * @param mask Prefix length (0-128)
   */
  IPv6Network(unsigned __int128 v, uint8_t mask) : IPNetwork(v, mask) {}
  
  /** @brief Get address family (always IPv6) */
  AddressFamily family() const override { return AddressFamily::IPv6; }
  
  /** @brief Convert network to address object */
  std::unique_ptr<IPAddress> toAddress() const override;
  
  /** @brief Clone the network object */
  std::unique_ptr<IPNetwork> cloneNetwork() const override;
  
  /** @brief Get CIDR string representation (e.g., "2001:db8::/64") */
  std::string toString() const override;
  
  /** @brief Clone as address object */
  std::unique_ptr<IPAddress> clone() const override;
  
  /** @brief Get network address as 128-bit unsigned integer */
  unsigned __int128 networkUint128() const { return value(); }
};
