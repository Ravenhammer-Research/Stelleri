#pragma once

#include "IPAddress.hpp"
#include <cstdint>
#include <memory>
#include <string>

class IPNetwork : public IPAddress {
protected:
  uint8_t mask_ = 0;

public:
  IPNetwork() = default;
  IPNetwork(unsigned __int128 v, uint8_t mask)
      : IPAddress(v, mask), mask_(mask) {}
  virtual ~IPNetwork() = default;

  uint8_t mask() const { return mask_; }

  // Returns the address object representing the network address (address
  // masked)
  virtual std::unique_ptr<IPAddress> toAddress() const = 0;

  // Clone the network object
  virtual std::unique_ptr<IPNetwork> cloneNetwork() const = 0;

  // Parse a CIDR string like "192.0.2.0/24" or "2001:db8::/64" and return an
  // IPNetwork
  static std::unique_ptr<IPNetwork> fromString(const std::string &s);
};
