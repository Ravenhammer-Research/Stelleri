#include "SystemConfigurationManager.hpp"
#include "ConfigData.hpp"
#include "IPv4Address.hpp"
#include "IPv4Network.hpp"
#include "IPv6Address.hpp"
#include "IPv6Network.hpp"
#include "InterfaceType.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <ifaddrs.h>
#include <map>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <utility>

// Minimal, safe stubs for system-backed configuration manager.
// Modifying system configuration is platform-specific and may require
// elevated privileges; keep these as explicit stubs for now.

void SystemConfigurationManager::set(const std::string & /*path*/,
                                     const ConfigData & /*data*/) {
  throw std::runtime_error("SystemConfigurationManager::set not implemented");
}

void SystemConfigurationManager::delete_config(const std::string & /*path*/) {
  throw std::runtime_error(
      "SystemConfigurationManager::delete_config not implemented");
}

static unsigned __int128 ipv6BytesToUint128(const unsigned char buf[16]) {
  unsigned __int128 v = 0;
  for (int i = 0; i < 16; ++i) {
    v <<= 8;
    v |= static_cast<unsigned __int128>(buf[i]);
  }
  return v;
}

static InterfaceType detectInterfaceTypeFromIfaddrs(const struct ifaddrs *ifa) {
  if (!ifa)
    return InterfaceType::Unknown;
  unsigned int flags = ifa->ifa_flags;
  if (flags & IFF_LOOPBACK)
    return InterfaceType::Loopback;
  if (flags & IFF_POINTOPOINT)
    return InterfaceType::PointToPoint;

  // Inspect link-layer sockaddr (AF_LINK / sockaddr_dl) on BSD-like systems.
  if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_LINK) {
    struct sockaddr_dl *sdl =
        reinterpret_cast<struct sockaddr_dl *>(ifa->ifa_addr);
    switch (sdl->sdl_type) {
    case IFT_ETHER:
    case IFT_FASTETHER:
    case IFT_GIGABITETHERNET:
    case IFT_FIBRECHANNEL:
    case IFT_AFLANE8023:
      return InterfaceType::Ethernet;
    case IFT_LOOP:
      return InterfaceType::Loopback;
    case IFT_PPP:
      return InterfaceType::PPP;
    case IFT_TUNNEL:
      return InterfaceType::Tunnel;
    case IFT_FDDI:
      return InterfaceType::FDDI;
    case IFT_ISO88025: /* token ring / token bus family */
    case IFT_ISO88023:
    case IFT_ISO88024:
    case IFT_ISO88026:
      return InterfaceType::TokenRing;
    case IFT_IEEE80211:
      return InterfaceType::Wireless;
    case IFT_BRIDGE:
      return InterfaceType::Bridge;
    case IFT_L2VLAN:
      return InterfaceType::VLAN;
    case IFT_ATM:
      return InterfaceType::ATM;
    case IFT_PROPVIRTUAL:
    case IFT_VIRTUALIPADDRESS:
      return InterfaceType::Virtual;
    default:
      return InterfaceType::Other;
    }
  }

  return InterfaceType::Unknown;
}

static std::optional<ConfigData> make_interface_from_addr(struct ifaddrs *ifa) {
  if (!ifa || !ifa->ifa_name)
    return std::nullopt;
  ConfigData cd;
  InterfaceConfig ic;
  ic.name = ifa->ifa_name;
  ic.type = detectInterfaceTypeFromIfaddrs(ifa);

  // Store interface flags
  if (ifa->ifa_flags) {
    ic.flags = ifa->ifa_flags;
  }

  // Get MTU using ioctl
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock >= 0) {
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFMTU, &ifr) == 0) {
      ic.mtu = ifr.ifr_mtu;
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
        auto addr =
            std::make_unique<IPv4Address>(static_cast<unsigned __int128>(v));
        ic.address = std::make_unique<IPv4Network>(addr->value(), 32);
      }
    } else if (ifa->ifa_addr->sa_family == AF_INET6) {
      struct sockaddr_in6 *a6 =
          reinterpret_cast<struct sockaddr_in6 *>(ifa->ifa_addr);
      if (inet_ntop(AF_INET6, &a6->sin6_addr, buf, sizeof(buf))) {
        unsigned __int128 v = ipv6BytesToUint128(
            reinterpret_cast<const unsigned char *>(a6->sin6_addr.s6_addr));
        auto addr6 = std::make_unique<IPv6Address>(v);
        ic.address = std::make_unique<IPv6Network>(addr6->value(), 128);
      }
    }
  }
  cd.iface.emplace(std::move(ic));
  return cd;
}

std::optional<ConfigData>
SystemConfigurationManager::getInterface(const std::string &name) const {
  struct ifaddrs *ifs = nullptr;
  if (getifaddrs(&ifs) != 0)
    return std::nullopt;
  std::optional<ConfigData> res = std::nullopt;
  for (struct ifaddrs *ifa = ifs; ifa; ifa = ifa->ifa_next) {
    if (!ifa->ifa_name)
      continue;
    if (name == ifa->ifa_name) {
      res = make_interface_from_addr(ifa);
      break;
    }
  }
  freeifaddrs(ifs);
  return res;
}

std::vector<ConfigData> SystemConfigurationManager::getInterfaces() const {
  std::vector<ConfigData> out;
  struct ifaddrs *ifs = nullptr;
  if (getifaddrs(&ifs) != 0)
    return out;
  std::map<std::string, ConfigData> seen;
  for (struct ifaddrs *ifa = ifs; ifa; ifa = ifa->ifa_next) {
    if (!ifa->ifa_name)
      continue;
    auto it = seen.find(ifa->ifa_name);
    if (it == seen.end()) {
      auto cdopt = make_interface_from_addr(ifa);
      if (cdopt)
        seen.try_emplace(ifa->ifa_name, std::move(*cdopt));
    } else {
      auto &cd = seen[ifa->ifa_name];
      // add alias if primary missing or append
      if (ifa->ifa_addr) {
        if (ifa->ifa_addr->sa_family == AF_INET) {
          struct sockaddr_in *a =
              reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr);
          uint32_t v = ntohl(a->sin_addr.s_addr);
          auto addr =
              std::make_unique<IPv4Address>(static_cast<unsigned __int128>(v));
          cd.iface->aliases.push_back(
              std::make_unique<IPv4Network>(addr->value(), 32));
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
          struct sockaddr_in6 *a6 =
              reinterpret_cast<struct sockaddr_in6 *>(ifa->ifa_addr);
          unsigned __int128 v = ipv6BytesToUint128(
              reinterpret_cast<const unsigned char *>(a6->sin6_addr.s6_addr));
          auto addr6 = std::make_unique<IPv6Address>(v);
          cd.iface->aliases.push_back(
              std::make_unique<IPv6Network>(addr6->value(), 128));
        }
      }
    }
  }
  freeifaddrs(ifs);

  for (auto &kv : seen)
    out.push_back(std::move(kv.second));
  return out;
}

std::vector<ConfigData> SystemConfigurationManager::getRoutes() const {
  std::vector<ConfigData> routes;

  // Use sysctl to fetch routing table on FreeBSD
  int mib[6];
  mib[0] = CTL_NET;
  mib[1] = PF_ROUTE;
  mib[2] = 0;
  mib[3] = AF_UNSPEC;
  mib[4] = NET_RT_DUMP;
  mib[5] = 0;

  size_t needed = 0;
  if (sysctl(mib, 6, nullptr, &needed, nullptr, 0) < 0) {
    return routes;
  }

  std::vector<char> buf(needed);
  if (sysctl(mib, 6, buf.data(), &needed, nullptr, 0) < 0) {
    return routes;
  }

  // Parse routing messages
  char *lim = buf.data() + needed;
  for (char *next = buf.data(); next < lim;) {
    struct rt_msghdr *rtm = reinterpret_cast<struct rt_msghdr *>(next);
    if (rtm->rtm_msglen == 0)
      break;

    // Skip non-route messages
    if (rtm->rtm_type != RTM_GET && rtm->rtm_type != RTM_ADD) {
      next += rtm->rtm_msglen;
      continue;
    }

    // Extract sockaddrs following the rt_msghdr
    struct sockaddr *sa = reinterpret_cast<struct sockaddr *>(rtm + 1);
    struct sockaddr *rti_info[RTAX_MAX];
    std::memset(rti_info, 0, sizeof(rti_info));

    // Parse sockaddrs based on rtm_addrs bitmask
    for (int i = 0;
         i < RTAX_MAX && reinterpret_cast<char *>(sa) < next + rtm->rtm_msglen;
         i++) {
      if ((rtm->rtm_addrs & (1 << i)) == 0)
        continue;
      rti_info[i] = sa;

// Move to next sockaddr (BSD sockaddrs have sa_len)
#define ROUNDUP(a)                                                             \
  ((a) > 0 ? (1 + (((a) - 1) | (sizeof(long) - 1))) : sizeof(long))
      sa = reinterpret_cast<struct sockaddr *>(reinterpret_cast<char *>(sa) +
                                               ROUNDUP(sa->sa_len));
    }

    // Create route entry
    ConfigData cd;
    RouteConfig rc;

    // Destination address (RTAX_DST)
    if (rti_info[RTAX_DST]) {
      struct sockaddr *dst = rti_info[RTAX_DST];
      char buf_dst[INET6_ADDRSTRLEN];
      int prefixlen = 0;

      if (dst->sa_family == AF_INET) {
        struct sockaddr_in *sin = reinterpret_cast<struct sockaddr_in *>(dst);
        inet_ntop(AF_INET, &sin->sin_addr, buf_dst, sizeof(buf_dst));

        // Get netmask if available
        if (rti_info[RTAX_NETMASK]) {
          struct sockaddr_in *mask =
              reinterpret_cast<struct sockaddr_in *>(rti_info[RTAX_NETMASK]);
          uint32_t m = ntohl(mask->sin_addr.s_addr);
          prefixlen = __builtin_popcount(m);
        } else {
          prefixlen = 32;
        }

        rc.prefix = std::string(buf_dst) + "/" + std::to_string(prefixlen);
      } else if (dst->sa_family == AF_INET6) {
        struct sockaddr_in6 *sin6 =
            reinterpret_cast<struct sockaddr_in6 *>(dst);
        inet_ntop(AF_INET6, &sin6->sin6_addr, buf_dst, sizeof(buf_dst));

        // Get netmask if available
        if (rti_info[RTAX_NETMASK]) {
          struct sockaddr_in6 *mask6 =
              reinterpret_cast<struct sockaddr_in6 *>(rti_info[RTAX_NETMASK]);
          prefixlen = 0;
          for (int j = 0; j < 16; j++) {
            prefixlen += __builtin_popcount(mask6->sin6_addr.s6_addr[j]);
          }
        } else {
          prefixlen = 128;
        }

        rc.prefix = std::string(buf_dst) + "/" + std::to_string(prefixlen);
      }
    }

    // Gateway address (RTAX_GATEWAY)
    if (rti_info[RTAX_GATEWAY]) {
      struct sockaddr *gw = rti_info[RTAX_GATEWAY];
      char buf_gw[INET6_ADDRSTRLEN];

      if (gw->sa_family == AF_INET) {
        struct sockaddr_in *sin = reinterpret_cast<struct sockaddr_in *>(gw);
        if (inet_ntop(AF_INET, &sin->sin_addr, buf_gw, sizeof(buf_gw))) {
          rc.nexthop = buf_gw;
        }
      } else if (gw->sa_family == AF_INET6) {
        struct sockaddr_in6 *sin6 = reinterpret_cast<struct sockaddr_in6 *>(gw);
        if (inet_ntop(AF_INET6, &sin6->sin6_addr, buf_gw, sizeof(buf_gw))) {
          rc.nexthop = buf_gw;
        }
      } else if (gw->sa_family == AF_LINK) {
        // Link-layer address (direct connection)
        struct sockaddr_dl *sdl = reinterpret_cast<struct sockaddr_dl *>(gw);
        if (sdl->sdl_nlen > 0) {
          rc.iface = std::string(sdl->sdl_data, sdl->sdl_nlen);
        }
      }
    }

    // Interface index
    if (rtm->rtm_index > 0) {
      char ifname[IF_NAMESIZE];
      if (if_indextoname(rtm->rtm_index, ifname)) {
        if (!rc.iface) {
          rc.iface = ifname;
        }
      }
    }

    // Flags
    if (rtm->rtm_flags & RTF_BLACKHOLE) {
      rc.blackhole = true;
    }

    // Only add routes with valid destinations
    if (!rc.prefix.empty()) {
      cd.route = std::move(rc);
      routes.push_back(std::move(cd));
    }

    next += rtm->rtm_msglen;
  }

  return routes;
}
