/**
 * @file IPNetwork.hpp
 * @brief IP network base class with CIDR notation
 */

#pragma once

#include "IPAddress.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>

class IPNetwork : public IPAddress {
protected:
  uint8_t mask_ = 0;

public:
  IPNetwork() = default;
    IPNetwork(unsigned __int128 v, uint8_t mask)
      : IPAddress(v), mask_(mask) {}
    explicit IPNetwork(const std::string &s);

  /** @brief Get prefix length/netmask bits */
  uint8_t mask() const { return mask_; }
  /** @brief Get textual representation of the address */
  virtual std::string toString() const = 0;
  IPNetwork(const std::string &s) {
    static const std::regex re(R"(^\s*(\S+)\/(\d{1,3})\s*$)");
    std::smatch m;
    if (!std::regex_match(s, m, re))
      throw std::invalid_argument("Invalid network string");

    std::string addrStr = m[1].str();
    std::string maskStr = m[2].str();

    unsigned long mask = 0;
    try {
      mask = std::stoul(maskStr);
    } catch (...) {
      throw std::invalid_argument("Invalid mask in network string");
    }

    struct in_addr a4;
    if (inet_pton(AF_INET, addrStr.c_str(), &a4) == 1) {
      if (mask > 32) throw std::invalid_argument("Invalid IPv4 mask");
      uint32_t v4 = ntohl(a4.s_addr);
      value_ = static_cast<unsigned __int128>(v4);
      mask_ = static_cast<uint8_t>(mask);
      return;
    }

    struct in6_addr a6;
    if (inet_pton(AF_INET6, addrStr.c_str(), &a6) == 1) {
      if (mask > 128) throw std::invalid_argument("Invalid IPv6 mask");
      unsigned __int128 v = 0;
      for (int i = 0; i < 16; ++i) {
        v <<= 8;
        v |= static_cast<unsigned char>(a6.s6_addr[i]);
      }
      value_ = v;
      mask_ = static_cast<uint8_t>(mask);
      return;
    }

    throw std::invalid_argument("Unknown address family in network string");
  }
};
