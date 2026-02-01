#include "InterfaceConfig.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "IPv4Address.hpp"
#include "IPv4Network.hpp"
#include "IPv6Address.hpp"
#include "IPv6Network.hpp"
#include "IPAddress.hpp"

InterfaceConfig::InterfaceConfig(const struct ifaddrs *ifa) {
  if (!ifa || !ifa->ifa_name)
    return;

  name = ifa->ifa_name;
  type = detectInterfaceTypeFromIfaddrs(ifa);

  // Store flags if present
  if (ifa->ifa_flags) {
    flags = ifa->ifa_flags;
  }

  // Get MTU
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock >= 0) {
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFMTU, &ifr) == 0) {
      mtu = ifr.ifr_mtu;
    }
    close(sock);
  }

  if (ifa->ifa_addr) {
    char buf[INET6_ADDRSTRLEN] = {0};
    if (ifa->ifa_addr->sa_family == AF_INET) {
      struct sockaddr_in *a =
          reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr);
      if (inet_ntop(AF_INET, &a->sin_addr, buf, sizeof(buf))) {
        uint32_t v = ntohl(a->sin_addr.s_addr);
        auto addr = std::make_unique<IPv4Address>(static_cast<unsigned __int128>(v));
        address = std::make_unique<IPv4Network>(addr->value(), 32);
      }
    } else if (ifa->ifa_addr->sa_family == AF_INET6) {
      struct sockaddr_in6 *a6 =
          reinterpret_cast<struct sockaddr_in6 *>(ifa->ifa_addr);
      if (inet_ntop(AF_INET6, &a6->sin6_addr, buf, sizeof(buf))) {
        unsigned __int128 v = IPAddress::ipv6BytesToUint128(
            reinterpret_cast<const unsigned char *>(a6->sin6_addr.s6_addr));
        auto addr6 = std::make_unique<IPv6Address>(v);
        address = std::make_unique<IPv6Network>(addr6->value(), 128);
      }
    }
  }
}

InterfaceConfig::InterfaceConfig(std::string name_,
                                 InterfaceType type_,
                                 std::unique_ptr<IPNetwork> address_,
                                 std::vector<std::unique_ptr<IPNetwork>> aliases_,
                                 std::unique_ptr<VRFConfig> vrf_,
                                 std::optional<uint32_t> flags_,
                                 std::optional<int> tunnel_vrf_,
                                 std::vector<std::string> groups_,
                                 std::optional<int> mtu_)
    : name(std::move(name_)),
      type(type_),
      address(std::move(address_)),
      aliases(std::move(aliases_)),
      vrf(std::move(vrf_)),
      flags(flags_),
      tunnel_vrf(tunnel_vrf_),
      groups(std::move(groups_)),
      mtu(mtu_) {}

void InterfaceConfig::save() const {
  // Do not create interfaces here; subclasses handle creation if necessary.
  if (!ConfigData::exists(name)) {
    throw std::runtime_error("Interface '" + name + "' does not exist");
  }

  // Configure address, MTU, flags, and other settings
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
  }

  // Configure addresses (primary + aliases) if provided
  if (address || !aliases.empty()) {
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
    configureAddresses(sock, ifr);
  }

  // Configure MTU if provided
  if (mtu) {
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
    configureMTU(sock, ifr);
  }

  // Bring interface up / configure flags

  struct ifreq ifr;
  std::memset(&ifr, 0, sizeof(ifr));
  std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
  configureFlags(sock, ifr);

  close(sock);

}

// Interface existence helper is provided by `ConfigData::exists`

void InterfaceConfig::configureAddresses(int sock, struct ifreq &ifr) const {
  auto handleNetwork = [&](const IPNetwork *net) {
    if (!net) return;
    if (net->family() == AddressFamily::IPv4) {
      auto v4net = dynamic_cast<const IPv4Network *>(net);
      if (!v4net) return;
      auto addr = std::make_unique<IPv4Address>(v4net->networkUint32());
      struct ifreq tmp_ifr;
      std::memset(&tmp_ifr, 0, sizeof(tmp_ifr));
      std::strncpy(tmp_ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
      struct sockaddr_in *a = reinterpret_cast<struct sockaddr_in *>(&tmp_ifr.ifr_addr);
      a->sin_family = AF_INET;
      a->sin_addr.s_addr = htonl(addr->asUint32());

      if (ioctl(sock, SIOCSIFADDR, &tmp_ifr) < 0) {
        close(sock);
        throw std::runtime_error("Failed to set interface address: " + std::string(strerror(errno)));
      }

      // Set netmask if prefix < 32
      if (v4net->mask() < 32) {
        struct ifreq mask_ifr;
        std::memset(&mask_ifr, 0, sizeof(mask_ifr));
        std::strncpy(mask_ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
        struct sockaddr_in *mask = reinterpret_cast<struct sockaddr_in *>(&mask_ifr.ifr_addr);
        mask->sin_family = AF_INET;
        uint32_t maskval = (v4net->mask() == 0) ? 0 : (~0u << (32 - v4net->mask()));
        mask->sin_addr.s_addr = htonl(maskval);

        if (ioctl(sock, SIOCSIFNETMASK, &mask_ifr) < 0) {
          close(sock);
          throw std::runtime_error("Failed to set netmask: " + std::string(strerror(errno)));
        }
      }
    } else if (net->family() == AddressFamily::IPv6) {
      std::cerr << "Warning: IPv6 address configuration not fully implemented\n";
    }
  };

  // primary
  if (address) handleNetwork(address.get());

  // aliases
  for (const auto &a : aliases) {
    if (a) handleNetwork(a.get());
  }
}

void InterfaceConfig::configureMTU(int sock, struct ifreq &ifr) const {
  ifr.ifr_mtu = *mtu;

  if (ioctl(sock, SIOCSIFMTU, &ifr) < 0) {
    close(sock);
    throw std::runtime_error("Failed to set MTU: " + std::string(strerror(errno)));
  }
}

void InterfaceConfig::configureFlags(int sock, struct ifreq &ifr) const {
  if (ioctl(sock, SIOCGIFFLAGS, &ifr) >= 0) {
    ifr.ifr_flags |= IFF_UP;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
      std::cerr << "Warning: Failed to bring interface up: " << strerror(errno) << "\n";
    }
  }
}

void InterfaceConfig::destroy() const {
  if (name.empty())
    throw std::runtime_error("InterfaceConfig::destroy(): empty interface name");

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
  }

  struct ifreq ifr;
  std::memset(&ifr, 0, sizeof(ifr));
  std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);

  if (ioctl(sock, SIOCIFDESTROY, &ifr) < 0) {
    int err = errno;
    close(sock);
    throw std::runtime_error("Failed to destroy interface '" + name + "': " + std::string(strerror(err)));
  }

  close(sock);
}
