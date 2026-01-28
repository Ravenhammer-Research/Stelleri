#pragma once

#include "IPNetwork.hpp"
#include "IPv6Address.hpp"
#include <cstdint>
#include <memory>

class IPv6Network : public IPNetwork {
public:
  IPv6Network(unsigned __int128 v, uint8_t mask) : IPNetwork(v, mask) {}
  AddressFamily family() const override { return AddressFamily::IPv6; }
  std::unique_ptr<IPAddress> toAddress() const override;
  std::unique_ptr<IPNetwork> cloneNetwork() const override;
  std::string toString() const override;
  std::unique_ptr<IPAddress> clone() const override;
  unsigned __int128 networkUint128() const { return value(); }
};
