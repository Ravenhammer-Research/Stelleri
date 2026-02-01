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
  IPv4Network(uint32_t v, uint8_t mask)
      : IPNetwork(static_cast<unsigned __int128>(v), mask) {}

  AddressFamily family() const override { return AddressFamily::IPv4; }
   inline std::string toString() const override {
     uint32_t v = static_cast<uint32_t>(value_);
     struct in_addr a4;
     a4.s_addr = htonl(v);
     char buf[INET_ADDRSTRLEN] = {0};
     if (inet_ntop(AF_INET, &a4, buf, sizeof(buf)) == nullptr)
       return std::string();
     return std::string(buf) + "/" + std::to_string(mask_);
   }
};
