/**
 * @file IPAddress.hpp
 * @brief IP address base class
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "AddressFamily.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <string>

/**
 * @brief Base class for IP addresses
 *
 * Uses 128-bit storage to handle both IPv4 and IPv6 addresses uniformly.
 * Derived classes implement family-specific behavior.
 */
class IPAddress {
protected:
  unsigned __int128 value_ = 0;

public:
  IPAddress() = default;
  explicit IPAddress(unsigned __int128 v) : value_(v) {}
  
  IPAddress::IPAddress(const std::string &s) {
  struct in_addr a4;
  if (inet_pton(AF_INET, s.c_str(), &a4) == 1) {
    uint32_t v4 = ntohl(a4.s_addr);
    value_ = static_cast<unsigned __int128>(v4);
    return;
  }

  struct in6_addr a6;
  if (inet_pton(AF_INET6, s.c_str(), &a6) == 1) {
    unsigned __int128 v = 0;
    for (int i = 0; i < 16; ++i) {
      v <<= 8;
      v |= static_cast<unsigned char>(a6.s6_addr[i]);
    }
    value_ = v;
    return;
  }

  value_ = 0;
}
  
  /** @brief Get address family (IPv4 or IPv6) */
  virtual AddressFamily family() const = 0;

  /** @brief Get textual representation of the address */
  virtual std::string toString() const = 0;
};


