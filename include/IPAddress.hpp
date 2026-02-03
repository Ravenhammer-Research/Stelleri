#pragma once

#include "AddressFamily.hpp"
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

// Polymorphic IP address base type
class IPAddress {
public:
  virtual ~IPAddress() = default;
  virtual AddressFamily family() const = 0;
  virtual std::string toString() const = 0;
  virtual std::unique_ptr<IPAddress> clone() const = 0;

  // Parse textual address (IPv4 or IPv6)
  static std::unique_ptr<IPAddress> fromString(const std::string &s);
};

// IPv4 concrete implementation
class IPv4Address : public IPAddress {
  uint32_t v_ = 0;

public:
  IPv4Address() = default;
  explicit IPv4Address(uint32_t v) : v_(v) {}
  explicit IPv4Address(const std::string &s) {
    struct in_addr a4;
    if (inet_pton(AF_INET, s.c_str(), &a4) == 1) {
      v_ = ntohl(a4.s_addr);
    } else {
      v_ = 0;
    }
  }

  AddressFamily family() const override { return AddressFamily::IPv4; }
  uint32_t value() const { return v_; }

  std::string toString() const override {
    struct in_addr a4;
    a4.s_addr = htonl(v_);
    char buf[INET_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET, &a4, buf, sizeof(buf)) == nullptr)
      return std::string();
    return std::string(buf);
  }

  std::unique_ptr<IPAddress> clone() const override {
    return std::make_unique<IPv4Address>(v_);
  }
};

// IPv6 concrete implementation
class IPv6Address : public IPAddress {
  unsigned __int128 v_ = 0;

public:
  IPv6Address() = default;
  explicit IPv6Address(unsigned __int128 v) : v_(v) {}
  explicit IPv6Address(const std::string &s) {
    struct in6_addr a6;
    if (inet_pton(AF_INET6, s.c_str(), &a6) == 1) {
      unsigned __int128 v = 0;
      for (int i = 0; i < 16; ++i) {
        v <<= 8;
        v |= static_cast<unsigned char>(a6.s6_addr[i]);
      }
      v_ = v;
    } else {
      v_ = 0;
    }
  }

  AddressFamily family() const override { return AddressFamily::IPv6; }
  unsigned __int128 value() const { return v_; }

  std::string toString() const override {
    struct in6_addr a6;
    unsigned __int128 v = v_;
    for (int i = 15; i >= 0; --i) {
      a6.s6_addr[i] = static_cast<uint8_t>(v & 0xFF);
      v >>= 8;
    }
    char buf[INET6_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET6, &a6, buf, sizeof(buf)) == nullptr)
      return std::string();
    return std::string(buf);
  }

  std::unique_ptr<IPAddress> clone() const override {
    return std::make_unique<IPv6Address>(v_);
  }
};

// Factory: parse an address string (IPv4 or IPv6)
inline std::unique_ptr<IPAddress> IPAddress::fromString(const std::string &s) {
  // Try IPv4
  struct in_addr a4;
  if (inet_pton(AF_INET, s.c_str(), &a4) == 1) {
    uint32_t v = ntohl(a4.s_addr);
    return std::make_unique<IPv4Address>(v);
  }
  // Try IPv6
  struct in6_addr a6;
  if (inet_pton(AF_INET6, s.c_str(), &a6) == 1) {
    unsigned __int128 v = 0;
    for (int i = 0; i < 16; ++i) {
      v <<= 8;
      v |= static_cast<unsigned char>(a6.s6_addr[i]);
    }
    return std::make_unique<IPv6Address>(v);
  }
  return nullptr;
}
