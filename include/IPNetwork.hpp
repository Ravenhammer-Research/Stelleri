/**
 * @file IPNetwork.hpp
 * @brief IP network base class with CIDR notation
 */

#pragma once

#include "IPAddress.hpp"
#include <cstdint>
#include <memory>
#include <string>

/**
 * @brief Base class for IP networks (address + prefix length)
 * 
 * Represents IP addresses with CIDR notation (e.g., 192.168.1.0/24).
 */
class IPNetwork : public IPAddress {
protected:
  uint8_t mask_ = 0;

public:
  IPNetwork() = default;
  IPNetwork(unsigned __int128 v, uint8_t mask)
      : IPAddress(v, mask), mask_(mask) {}
  virtual ~IPNetwork() = default;

  /** @brief Get prefix length/netmask bits */
  uint8_t mask() const { return mask_; }

  /**
   * @brief Convert network to address object
   * @return Address representation of the network
   */
  virtual std::unique_ptr<IPAddress> toAddress() const = 0;

  /** @brief Clone the network object */
  virtual std::unique_ptr<IPNetwork> cloneNetwork() const = 0;

  /**
   * @brief Parse a CIDR string
   * @param s Network string (e.g., "192.0.2.0/24" or "2001:db8::/64")
   * @return Unique pointer to IPNetwork, or nullptr if invalid
   */
  static std::unique_ptr<IPNetwork> fromString(const std::string &s);
};
