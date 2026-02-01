/**
 * @file IPv6Address.hpp
 * @brief IPv6 address implementation
 */

#pragma once

#include "IPAddress.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>

/**
 * @brief IPv6 address (128-bit)
 *
 * Stores IPv6 addresses as 128-bit unsigned integers.
 * Supports standard colon-hexadecimal notation with compression (e.g.,
 * "2001:db8::1").
 */
class IPv6Address : public IPAddress {
public:
  explicit IPv6Address(unsigned __int128 v) : IPAddress(v) {}
  AddressFamily family() const override { return AddressFamily::IPv6; }

  std::string toString() const override {
  struct in6_addr a6;
  std::memset(&a6, 0, sizeof(a6));
  unsigned __int128 v = value_;
  for (int i = 15; i >= 0; --i) {
    a6.s6_addr[i] = static_cast<uint8_t>(v & 0xFF);
    v >>= 8;
  }
  char buf[INET6_ADDRSTRLEN] = {0};
  if (inet_ntop(AF_INET6, &a6, buf, sizeof(buf)) == nullptr)
    return std::string();
  return std::string(buf);
}
};
