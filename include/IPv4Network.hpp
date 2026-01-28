#pragma once

#include "IPNetwork.hpp"
#include "IPv4Address.hpp"
#include <cstdint>
#include <memory>

class IPv4Network : public IPNetwork {
public:
  IPv4Network(uint32_t v, uint8_t mask)
      : IPNetwork(static_cast<unsigned __int128>(v), mask) {}
  IPv4Network(unsigned __int128 v, uint8_t mask) : IPNetwork(v, mask) {}
  AddressFamily family() const override { return AddressFamily::IPv4; }
  std::unique_ptr<IPAddress> toAddress() const override;
  std::unique_ptr<IPNetwork> cloneNetwork() const override;
  std::string toString() const override;
  std::unique_ptr<IPAddress> clone() const override;
  uint32_t networkUint32() const { return static_cast<uint32_t>(value()); }
};
