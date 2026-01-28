#include "IPv6Address.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>

static void uint128ToIpv6Bytes(unsigned __int128 v, unsigned char out[16]) {
  for (int i = 15; i >= 0; --i) {
    out[i] = static_cast<unsigned char>(v & 0xFFu);
    v >>= 8;
  }
}

std::string IPv6Address::toString() const {
  unsigned char bytes[16];
  uint128ToIpv6Bytes(asUint128(), bytes);
  in6_addr a6;
  std::memcpy(a6.s6_addr, bytes, 16);
  char buf[INET6_ADDRSTRLEN] = {0};
  if (inet_ntop(AF_INET6, &a6, buf, sizeof(buf)) == nullptr)
    return std::string();
  return std::string(buf);
}

std::unique_ptr<IPAddress> IPv6Address::clone() const {
  return std::make_unique<IPv6Address>(asUint128());
}
