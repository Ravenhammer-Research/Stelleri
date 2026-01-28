#pragma once

#include "IPAddress.hpp"
#include <cstdint>
#include <memory>
#include <string>

class IPv6Address : public IPAddress {
public:
  explicit IPv6Address(unsigned __int128 v) : IPAddress(v) {}
  AddressFamily family() const override { return AddressFamily::IPv6; }
  unsigned __int128 asUint128() const { return value(); }
  std::string toString() const override;
  std::unique_ptr<IPAddress> clone() const override;
};
