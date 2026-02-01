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
  IPv6Network(unsigned __int128 v, uint8_t mask) : IPNetwork(v, mask) {}
  AddressFamily family() const override { return AddressFamily::IPv6; }
  std::string toString() const override {
  unsigned __int128 v = value_;
  struct in6_addr a6;
  for (int i = 0; i < 16; ++i) {
    a6.s6_addr[15 - i] = static_cast<uint8_t>(v & 0xFF);
    v >>= 8;
  }
  char buf[INET6_ADDRSTRLEN] = {0};
  if (inet_ntop(AF_INET6, &a6, buf, sizeof(buf)) == nullptr)
    return std::string();
  return std::string(buf) + "/" + std::to_string(mask_);
}
};
