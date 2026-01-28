#include "IPAddress.hpp"
#include "IPv4Address.hpp"
#include "IPv6Address.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>

namespace {
  static unsigned __int128 ipv6BytesToUint128(const unsigned char buf[16]) {
    unsigned __int128 v = 0;
    for (int i = 0; i < 16; ++i) {
      v <<= 8;
      v |= static_cast<unsigned __int128>(buf[i]);
    }
    return v;
  }
} // anonymous namespace

std::unique_ptr<IPAddress> IPAddress::fromString(const std::string &s) {
  in_addr a4;
  if (inet_pton(AF_INET, s.c_str(), &a4) == 1) {
    uint32_t v = ntohl(a4.s_addr);
    return std::make_unique<IPv4Address>(v);
  }
  in6_addr a6;
  if (inet_pton(AF_INET6, s.c_str(), &a6) == 1) {
    unsigned __int128 v =
        ipv6BytesToUint128(reinterpret_cast<const unsigned char *>(a6.s6_addr));
    return std::make_unique<IPv6Address>(v);
  }
  return nullptr;
}
