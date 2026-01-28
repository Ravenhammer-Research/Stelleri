#pragma once

#include "IPAddress.hpp"
#include <cstdint>
#include <memory>
#include <string>

class IPv4Address : public IPAddress {
public:
  explicit IPv4Address(uint32_t v)
      : IPAddress(static_cast<unsigned __int128>(v)) {}
  explicit IPv4Address(unsigned __int128 v) : IPAddress(v) {}
  AddressFamily family() const override { return AddressFamily::IPv4; }
  uint32_t asUint32() const { return static_cast<uint32_t>(value()); }
  std::string toString() const override;
  std::unique_ptr<IPAddress> clone() const override;
};
