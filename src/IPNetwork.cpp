#include "IPNetwork.hpp"
#include "IPAddress.hpp"
#include "IPv4Address.hpp"
#include "IPv4Network.hpp"
#include "IPv6Address.hpp"
#include "IPv6Network.hpp"
#include <regex>
#include <stdexcept>
#include <string>

std::unique_ptr<IPNetwork> IPNetwork::fromString(const std::string &s) {
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

  std::unique_ptr<IPAddress> addr = IPAddress::fromString(addrStr);
  if (!addr)
    throw std::invalid_argument("Invalid address in network string");

  if (addr->family() == AddressFamily::IPv4) {
    if (mask > 32)
      throw std::invalid_argument("Invalid IPv4 mask");
    auto v4 = dynamic_cast<IPv4Address *>(addr.get());
    uint32_t a = v4->asUint32();
    uint32_t net;
    if (mask == 0)
      net = 0;
    else if (mask == 32)
      net = a;
    else {
      uint32_t hostmask = ((mask >= 32) ? 0u : ((1u << (32 - mask)) - 1u));
      net = a & ~hostmask;
    }
    return std::make_unique<IPv4Network>(net, static_cast<uint8_t>(mask));
  } else if (addr->family() == AddressFamily::IPv6) {
    if (mask > 128)
      throw std::invalid_argument("Invalid IPv6 mask");
    auto v6 = dynamic_cast<IPv6Address *>(addr.get());
    unsigned __int128 a = v6->asUint128();
    unsigned __int128 net;
    if (mask == 0)
      net = 0;
    else if (mask == 128)
      net = a;
    else {
      unsigned __int128 hostmask =
          (((unsigned __int128)1 << (128 - mask)) - (unsigned __int128)1);
      net = a & ~hostmask;
    }
    return std::make_unique<IPv6Network>(net, static_cast<uint8_t>(mask));
  }

  throw std::invalid_argument("Unknown address family in network string");
}
