#pragma once

#include "AddressFamily.hpp"
#include "IPAddress.hpp"
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

// Polymorphic IP network base type
class IPNetwork {
public:
  virtual ~IPNetwork() = default;
  virtual AddressFamily family() const = 0;
  virtual std::unique_ptr<IPAddress> address() const = 0;
  virtual uint8_t mask() const = 0;
  virtual std::unique_ptr<IPAddress> subnet() const = 0;
  virtual std::string toString() const = 0;
  virtual std::unique_ptr<IPNetwork> clone() const = 0;
  // Compatibility helper used by older call sites
  std::unique_ptr<IPNetwork> cloneNetwork() const { return clone(); }

  // Parse network string like "192.0.2.0/24" or IPv6 notation.
  static std::unique_ptr<IPNetwork> fromString(const std::string &s);
};

// IPv4 network implementation
class IPv4Network : public IPNetwork {
  uint32_t addr_ = 0;
  uint8_t mask_ = 32;

public:
  IPv4Network() = default;
  IPv4Network(uint32_t a, uint8_t m) : addr_(a), mask_(m) {}
  explicit IPv4Network(const std::string &s) {
    auto pos = s.find('/');
    std::string a = s;
    if (pos != std::string::npos) {
      a = s.substr(0, pos);
      mask_ = static_cast<uint8_t>(std::stoi(s.substr(pos + 1)));
    }
    struct in_addr a4;
    if (inet_pton(AF_INET, a.c_str(), &a4) == 1) {
      addr_ = ntohl(a4.s_addr);
    } else {
      addr_ = 0;
    }
  }

  AddressFamily family() const override { return AddressFamily::IPv4; }
  std::unique_ptr<IPAddress> address() const override {
    return std::make_unique<IPv4Address>(addr_);
  }
  uint8_t mask() const override { return mask_; }

  std::unique_ptr<IPAddress> subnet() const override {
    uint32_t maskVal = (mask_ == 0) ? 0u : (~uint32_t(0) << (32 - mask_));
    return std::make_unique<IPv4Address>(maskVal);
  }

  std::string toString() const override {
    struct in_addr a4;
    a4.s_addr = htonl(addr_);
    char buf[INET_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET, &a4, buf, sizeof(buf)) == nullptr)
      return std::string();
    return std::string(buf) + "/" + std::to_string(mask_);
  }

  std::unique_ptr<IPNetwork> clone() const override {
    return std::make_unique<IPv4Network>(addr_, mask_);
  }
};

// IPv6 network implementation
class IPv6Network : public IPNetwork {
  unsigned __int128 addr_ = 0;
  uint8_t mask_ = 128;

public:
  IPv6Network() = default;
  IPv6Network(unsigned __int128 a, uint8_t m) : addr_(a), mask_(m) {}
  explicit IPv6Network(const std::string &s) {
    auto pos = s.find('/');
    std::string a = s;
    if (pos != std::string::npos) {
      a = s.substr(0, pos);
      mask_ = static_cast<uint8_t>(std::stoi(s.substr(pos + 1)));
    }
    struct in6_addr a6;
    if (inet_pton(AF_INET6, a.c_str(), &a6) == 1) {
      unsigned __int128 v = 0;
      for (int i = 0; i < 16; ++i) {
        v <<= 8;
        v |= static_cast<unsigned char>(a6.s6_addr[i]);
      }
      addr_ = v;
    } else {
      addr_ = 0;
    }
  }

  AddressFamily family() const override { return AddressFamily::IPv6; }
  std::unique_ptr<IPAddress> address() const override {
    return std::make_unique<IPv6Address>(addr_);
  }
  uint8_t mask() const override { return mask_; }

  std::unique_ptr<IPAddress> subnet() const override {
    unsigned __int128 maskVal = 0;
    if (mask_ == 0) {
      maskVal = 0;
    } else {
      unsigned __int128 allOnes = ~static_cast<unsigned __int128>(0);
      maskVal = (mask_ == 128) ? allOnes : (allOnes << (128 - mask_));
    }
    return std::make_unique<IPv6Address>(maskVal);
  }

  std::string toString() const override {
    struct in6_addr a6;
    unsigned __int128 v = addr_;
    for (int i = 15; i >= 0; --i) {
      a6.s6_addr[i] = static_cast<uint8_t>(v & 0xFF);
      v >>= 8;
    }
    char buf[INET6_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET6, &a6, buf, sizeof(buf)) == nullptr)
      return std::string();
    return std::string(buf) + "/" + std::to_string(mask_);
  }

  std::unique_ptr<IPNetwork> clone() const override {
    return std::make_unique<IPv6Network>(addr_, mask_);
  }
};

// Factory: parse network string like "192.0.2.0/24" or IPv6
inline std::unique_ptr<IPNetwork> IPNetwork::fromString(const std::string &s) {
  // Try IPv4 pattern
  auto pos = s.find('/');
  std::string addr = s;
  if (pos != std::string::npos)
    addr = s.substr(0, pos);
  struct in_addr a4;
  if (inet_pton(AF_INET, addr.c_str(), &a4) == 1) {
    uint32_t v = ntohl(a4.s_addr);
    uint8_t m = 32;
    if (pos != std::string::npos)
      m = static_cast<uint8_t>(std::stoi(s.substr(pos + 1)));
    return std::make_unique<IPv4Network>(v, m);
  }
  struct in6_addr a6;
  if (inet_pton(AF_INET6, addr.c_str(), &a6) == 1) {
    unsigned __int128 v = 0;
    for (int i = 0; i < 16; ++i) {
      v <<= 8;
      v |= static_cast<unsigned char>(a6.s6_addr[i]);
    }
    uint8_t m = 128;
    if (pos != std::string::npos)
      m = static_cast<uint8_t>(std::stoi(s.substr(pos + 1)));
    return std::make_unique<IPv6Network>(v, m);
  }
  return nullptr;
}
