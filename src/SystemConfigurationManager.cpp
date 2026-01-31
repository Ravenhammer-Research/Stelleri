#include "SystemConfigurationManager.hpp"
#include "ConfigData.hpp"
#include "IPv4Address.hpp"
#include "IPv4Network.hpp"
#include "IPv6Address.hpp"
#include "IPv6Network.hpp"
#include "InterfaceType.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <ifaddrs.h>
#include <iostream>
#include <map>
#include <net/if.h>
#include <net/if_bridgevar.h>
#include <net/if_dl.h>
#include <net/if_lagg.h>
#include <net/if_types.h>
#include <net/if_vlan_var.h>
#include <net/route.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <utility>

// FreeBSD system configuration using ioctls
// Requires elevated privileges (root) for most operations

void SystemConfigurationManager::set([[maybe_unused]] const std::string &path,
                                     const ConfigData &data) {
  // Handle interface configuration
  if (data.iface) {
    const auto &iface = *data.iface;
    
    // Check if interface exists
    bool exists = false;
    struct ifaddrs *ifs = nullptr;
    if (getifaddrs(&ifs) == 0) {
      for (struct ifaddrs *ifa = ifs; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_name && iface.name == ifa->ifa_name) {
          exists = true;
          break;
        }
      }
      freeifaddrs(ifs);
    }
    
    // If interface doesn't exist, check if it's a type we can create
    if (!exists) {
      bool canCreate = false;
      switch (iface.type) {
        case InterfaceType::Loopback:
        case InterfaceType::Bridge:
        case InterfaceType::VLAN:
        case InterfaceType::Tunnel:
        case InterfaceType::Virtual:
          canCreate = true;
          break;
        case InterfaceType::Ethernet:
        case InterfaceType::Wireless:
        case InterfaceType::PPP:
        case InterfaceType::PointToPoint:
        default:
          canCreate = false;
          break;
      }
      
      if (!canCreate) {
        throw std::runtime_error(
            "Cannot create interface '" + iface.name + 
            "': Physical/hardware interfaces cannot be created via CLI. " +
            "Interface does not exist or is not attached.");
      }
      
      // Create the interface
      createInterface(iface.name, iface.type);
    }
    
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
      throw std::runtime_error("Failed to create socket: " + 
                               std::string(strerror(errno)));
    }

    // Configure address if provided
    if (iface.address) {
      struct ifreq ifr;
      std::memset(&ifr, 0, sizeof(ifr));
      std::strncpy(ifr.ifr_name, iface.name.c_str(), IFNAMSIZ - 1);

      if (iface.address->family() == AddressFamily::IPv4) {
        auto v4addr = dynamic_cast<const IPv4Address *>(iface.address.get());
        if (v4addr) {
          struct sockaddr_in *addr = 
              reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr);
          addr->sin_family = AF_INET;
          addr->sin_addr.s_addr = htonl(v4addr->asUint32());
          
          if (ioctl(sock, SIOCSIFADDR, &ifr) < 0) {
            close(sock);
            throw std::runtime_error("Failed to set interface address: " + 
                                   std::string(strerror(errno)));
          }

          // Set netmask
          auto v4net = dynamic_cast<const IPv4Network *>(iface.address.get());
          if (v4net && v4net->mask() < 32) {
            std::memset(&ifr, 0, sizeof(ifr));
            std::strncpy(ifr.ifr_name, iface.name.c_str(), IFNAMSIZ - 1);
            struct sockaddr_in *mask = 
                reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr);
            mask->sin_family = AF_INET;
            uint32_t maskval = (v4net->mask() == 0) ? 0 : 
                               (~0u << (32 - v4net->mask()));
            mask->sin_addr.s_addr = htonl(maskval);
            
            if (ioctl(sock, SIOCSIFNETMASK, &ifr) < 0) {
              close(sock);
              throw std::runtime_error("Failed to set netmask: " + 
                                     std::string(strerror(errno)));
            }
          }
        }
      } else if (iface.address->family() == AddressFamily::IPv6) {
        // IPv6 configuration requires different approach
        close(sock);
        sock = socket(AF_INET6, SOCK_DGRAM, 0);
        if (sock < 0) {
          throw std::runtime_error("Failed to create IPv6 socket: " + 
                                 std::string(strerror(errno)));
        }

        struct in6_ifreq {
          struct in6_addr ifr6_addr;
          uint32_t ifr6_prefixlen;
          unsigned int ifr6_ifindex;
        };
        
        // Note: FreeBSD IPv6 address configuration typically requires
        // more complex handling via routing sockets. This is a simplified stub.
        std::cerr << "Warning: IPv6 address configuration not fully implemented\n";
      }
    }

    // Configure MTU if provided
    if (iface.mtu) {
      struct ifreq ifr;
      std::memset(&ifr, 0, sizeof(ifr));
      std::strncpy(ifr.ifr_name, iface.name.c_str(), IFNAMSIZ - 1);
      ifr.ifr_mtu = *iface.mtu;
      
      if (ioctl(sock, SIOCSIFMTU, &ifr) < 0) {
        close(sock);
        throw std::runtime_error("Failed to set MTU: " + 
                               std::string(strerror(errno)));
      }
    }

    // Bring interface up
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, iface.name.c_str(), IFNAMSIZ - 1);
    
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) >= 0) {
      ifr.ifr_flags |= IFF_UP;
      if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
        std::cerr << "Warning: Failed to bring interface up: " 
                  << strerror(errno) << "\n";
      }
    }

    close(sock);

    // Configure bridge-specific settings
    if (iface.bridge) {
      configureBridge(iface.name, *iface.bridge);
    }

    // Configure LAGG-specific settings
    if (iface.lagg) {
      configureLagg(iface.name, *iface.lagg);
    }

    // Configure VLAN-specific settings
    if (iface.vlan) {
      configureVLAN(iface.name, *iface.vlan);
    }

    // Configure GIF/tunnel endpoints
    if (iface.tunnel && iface.tunnel->source && iface.tunnel->destination) {
      configureGIFTunnel(iface.name, *iface.tunnel->source,
                        *iface.tunnel->destination, iface.tunnel_vrf);
    }
  }

  // Handle route configuration
  if (data.route) {
    const auto &route = *data.route;
    
    // Parse prefix
    auto network = IPNetwork::fromString(route.prefix);
    if (!network) {
      throw std::runtime_error("Invalid route prefix: " + route.prefix);
    }

    // Create routing socket
    int sock = socket(PF_ROUTE, SOCK_RAW, AF_UNSPEC);
    if (sock < 0) {
      throw std::runtime_error("Failed to create routing socket: " + 
                             std::string(strerror(errno)));
    }

    // Build routing message for RTM_ADD
    struct {
      struct rt_msghdr hdr;
      struct sockaddr_storage addrs[RTAX_MAX];
    } rtmsg;
    
    std::memset(&rtmsg, 0, sizeof(rtmsg));
    
    rtmsg.hdr.rtm_msglen = sizeof(struct rt_msghdr);
    rtmsg.hdr.rtm_version = RTM_VERSION;
    rtmsg.hdr.rtm_type = RTM_ADD;
    rtmsg.hdr.rtm_flags = RTF_UP | RTF_STATIC;
    rtmsg.hdr.rtm_addrs = RTA_DST | RTA_NETMASK;
    rtmsg.hdr.rtm_pid = getpid();
    rtmsg.hdr.rtm_seq = 1;

    char *cp = reinterpret_cast<char *>(&rtmsg) + sizeof(struct rt_msghdr);

    if (network->family() == AddressFamily::IPv4) {
      auto v4net = dynamic_cast<IPv4Network *>(network.get());
      
      // Destination
      struct sockaddr_in dst;
      std::memset(&dst, 0, sizeof(dst));
      dst.sin_len = sizeof(dst);
      dst.sin_family = AF_INET;
      dst.sin_addr.s_addr = htonl(v4net->networkUint32());
      std::memcpy(cp, &dst, sizeof(dst));
      cp += sizeof(dst);
      rtmsg.hdr.rtm_msglen += sizeof(dst);

      // Netmask
      struct sockaddr_in mask;
      std::memset(&mask, 0, sizeof(mask));
      mask.sin_len = sizeof(mask);
      mask.sin_family = AF_INET;
      uint32_t maskval = (v4net->mask() == 0) ? 0 : (~0u << (32 - v4net->mask()));
      mask.sin_addr.s_addr = htonl(maskval);
      std::memcpy(cp, &mask, sizeof(mask));
      cp += sizeof(mask);
      rtmsg.hdr.rtm_msglen += sizeof(mask);

      // Gateway if provided
      if (route.nexthop) {
        auto gw = IPAddress::fromString(*route.nexthop);
        if (gw && gw->family() == AddressFamily::IPv4) {
          auto v4gw = dynamic_cast<IPv4Address *>(gw.get());
          struct sockaddr_in gateway;
          std::memset(&gateway, 0, sizeof(gateway));
          gateway.sin_len = sizeof(gateway);
          gateway.sin_family = AF_INET;
          gateway.sin_addr.s_addr = htonl(v4gw->asUint32());
          std::memcpy(cp, &gateway, sizeof(gateway));
          cp += sizeof(gateway);
          rtmsg.hdr.rtm_msglen += sizeof(gateway);
          rtmsg.hdr.rtm_addrs |= RTA_GATEWAY;
          rtmsg.hdr.rtm_flags |= RTF_GATEWAY;
        }
      }

      // Blackhole flag
      if (route.blackhole) {
        rtmsg.hdr.rtm_flags |= RTF_BLACKHOLE;
      }
      
      // Reject flag
      if (route.reject) {
        rtmsg.hdr.rtm_flags |= RTF_REJECT;
      }
    }

    // Send routing message
    if (write(sock, &rtmsg, rtmsg.hdr.rtm_msglen) < 0) {
      close(sock);
      if (errno == EEXIST) {
        throw std::runtime_error("Route already exists");
      }
      throw std::runtime_error("Failed to add route: " + 
                             std::string(strerror(errno)));
    }

    close(sock);
  }

  // Handle VRF configuration
  if (data.vrf) {
    const auto &vrf = *data.vrf;
    // VRF on FreeBSD is managed via FIB (Forwarding Information Base)
    // This requires setting up FIB routing tables
    // For now, just acknowledge the configuration
    std::cerr << "Note: VRF/FIB configuration requires kernel support (setfib)\n";
    std::cerr << "VRF '" << vrf.name << "' registered";
    if (vrf.table) {
      std::cerr << " with FIB table " << *vrf.table;
    }
    std::cerr << "\n";
  }
}

void SystemConfigurationManager::delete_config(const std::string &path) {
  // Parse path to determine what to delete
  if (path.find("interfaces.") == 0) {
    // Extract interface name
    std::string ifname = path.substr(11); // Skip "interfaces."
    
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
      throw std::runtime_error("Failed to create socket: " + 
                             std::string(strerror(errno)));
    }

    // Bring interface down
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);
    
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) >= 0) {
      ifr.ifr_flags &= ~IFF_UP;
      if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
        close(sock);
        throw std::runtime_error("Failed to bring interface down: " + 
                               std::string(strerror(errno)));
      }
    }

    // Clear IP address
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);
    struct sockaddr_in *addr = 
        reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr);
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    
    ioctl(sock, SIOCSIFADDR, &ifr); // Ignore errors
    
    close(sock);
  } else if (path.find("route.") == 0) {
    // Extract route prefix
    std::string prefix = path.substr(6); // Skip "route."
    
    auto network = IPNetwork::fromString(prefix);
    if (!network) {
      throw std::runtime_error("Invalid route prefix: " + prefix);
    }

    int sock = socket(PF_ROUTE, SOCK_RAW, AF_UNSPEC);
    if (sock < 0) {
      throw std::runtime_error("Failed to create routing socket: " + 
                             std::string(strerror(errno)));
    }

    // Build routing message for RTM_DELETE
    struct {
      struct rt_msghdr hdr;
      struct sockaddr_storage addrs[RTAX_MAX];
    } rtmsg;
    
    std::memset(&rtmsg, 0, sizeof(rtmsg));
    
    rtmsg.hdr.rtm_msglen = sizeof(struct rt_msghdr);
    rtmsg.hdr.rtm_version = RTM_VERSION;
    rtmsg.hdr.rtm_type = RTM_DELETE;
    rtmsg.hdr.rtm_addrs = RTA_DST | RTA_NETMASK;
    rtmsg.hdr.rtm_pid = getpid();
    rtmsg.hdr.rtm_seq = 1;

    char *cp = reinterpret_cast<char *>(&rtmsg) + sizeof(struct rt_msghdr);

    if (network->family() == AddressFamily::IPv4) {
      auto v4net = dynamic_cast<IPv4Network *>(network.get());
      
      // Destination
      struct sockaddr_in dst;
      std::memset(&dst, 0, sizeof(dst));
      dst.sin_len = sizeof(dst);
      dst.sin_family = AF_INET;
      dst.sin_addr.s_addr = htonl(v4net->networkUint32());
      std::memcpy(cp, &dst, sizeof(dst));
      cp += sizeof(dst);
      rtmsg.hdr.rtm_msglen += sizeof(dst);

      // Netmask
      struct sockaddr_in mask;
      std::memset(&mask, 0, sizeof(mask));
      mask.sin_len = sizeof(mask);
      mask.sin_family = AF_INET;
      uint32_t maskval = (v4net->mask() == 0) ? 0 : (~0u << (32 - v4net->mask()));
      mask.sin_addr.s_addr = htonl(maskval);
      std::memcpy(cp, &mask, sizeof(mask));
      rtmsg.hdr.rtm_msglen += sizeof(mask);
    }

    if (write(sock, &rtmsg, rtmsg.hdr.rtm_msglen) < 0) {
      close(sock);
      if (errno == ESRCH) {
        throw std::runtime_error("Route not found");
      }
      throw std::runtime_error("Failed to delete route: " + 
                             std::string(strerror(errno)));
    }

    close(sock);
  } else if (path.find("vrf.") == 0) {
    std::string vrfname = path.substr(4); // Skip "vrf."
    // VRF/FIB tables on FreeBSD are kernel routing tables and cannot be deleted
    // They exist as long as the kernel runs. You can only stop using them or
    // clear their routes, but the FIB table itself remains.
    throw std::runtime_error(
        "Cannot delete VRF '" + vrfname + 
        "': FreeBSD FIB tables are permanent kernel structures");
  } else {
    throw std::runtime_error("Unknown configuration path: " + path);
  }
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

void SystemConfigurationManager::createInterface(
    const std::string &name, InterfaceType type) {
  // FreeBSD interface creation using SIOCIFCREATE ioctl
  // Interface type is determined by name prefix (bridge*, gif*, vlan*, epair*, etc.)
  
  // Validate that the name prefix matches the expected type
  std::string prefix;
  switch (type) {
  case InterfaceType::Bridge:
    prefix = "bridge";
    break;
  case InterfaceType::VLAN:
    prefix = "vlan";
    break;
  case InterfaceType::Tunnel:
    prefix = "gif"; // GIF is the most common tunnel type
    break;
  case InterfaceType::Virtual:
    prefix = "epair"; // epair, tap, tun are common virtual types
    break;
  case InterfaceType::Loopback:
    prefix = "lo";
    break;
  default:
    throw std::runtime_error("Cannot create interface of type: " +
                             to_string(type));
  }

  // Warn if name doesn't match expected prefix (but allow it)
  if (name.compare(0, prefix.length(), prefix) != 0 && 
      !(type == InterfaceType::Virtual && 
        (name.compare(0, 3, "tap") == 0 || name.compare(0, 3, "tun") == 0))) {
    std::cerr << "Warning: Interface name '" << name 
              << "' doesn't match expected prefix for type " << to_string(type)
              << " (expected '" << prefix << "*')\n";
  }

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Failed to create socket: " +
                             std::string(strerror(errno)));
  }

  struct ifreq ifr;
  std::memset(&ifr, 0, sizeof(ifr));
  std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);

  if (ioctl(sock, SIOCIFCREATE, &ifr) < 0) {
    close(sock);
    throw std::runtime_error("Failed to create interface '" + name +
                             "': " + std::string(strerror(errno)));
  }

  close(sock);
}

void SystemConfigurationManager::configureBridge(
    const std::string &name, const BridgeInterfaceConfig &config) {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Failed to create socket: " +
                             std::string(strerror(errno)));
  }

  // Add members to bridge (simple list)
  for (const auto &member : config.members) {
    struct ifbreq req;
    std::memset(&req, 0, sizeof(req));
    std::strncpy(req.ifbr_ifsname, member.c_str(), IFNAMSIZ - 1);

    struct ifdrv ifd;
    std::memset(&ifd, 0, sizeof(ifd));
    std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
    ifd.ifd_cmd = BRDGADD;
    ifd.ifd_len = sizeof(req);
    ifd.ifd_data = &req;

    if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
      close(sock);
      throw std::runtime_error("Failed to add member '" + member +
                               "' to bridge '" + name +
                               "': " + std::string(strerror(errno)));
    }
  }

  // Add and configure detailed member configs
  for (const auto &member : config.member_configs) {
    // Add member to bridge
    struct ifbreq req;
    std::memset(&req, 0, sizeof(req));
    std::strncpy(req.ifbr_ifsname, member.name.c_str(), IFNAMSIZ - 1);

    struct ifdrv ifd;
    std::memset(&ifd, 0, sizeof(ifd));
    std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
    ifd.ifd_cmd = BRDGADD;
    ifd.ifd_len = sizeof(req);
    ifd.ifd_data = &req;

    if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
      close(sock);
      throw std::runtime_error("Failed to add member '" + member.name +
                               "' to bridge '" + name +
                               "': " + std::string(strerror(errno)));
    }

    // Configure member port flags
    uint32_t flags = 0;
    if (member.stp) flags |= IFBIF_STP;
    if (member.edge) flags |= IFBIF_BSTP_EDGE;
    if (member.autoedge) flags |= IFBIF_BSTP_AUTOEDGE;
    if (member.ptp) flags |= IFBIF_BSTP_PTP;
    if (member.autoptp) flags |= IFBIF_BSTP_AUTOPTP;

    if (flags > 0) {
      std::memset(&req, 0, sizeof(req));
      std::strncpy(req.ifbr_ifsname, member.name.c_str(), IFNAMSIZ - 1);
      req.ifbr_ifsflags = flags;

      std::memset(&ifd, 0, sizeof(ifd));
      std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
      ifd.ifd_cmd = BRDGSIFFLGS;
      ifd.ifd_len = sizeof(req);
      ifd.ifd_data = &req;

      if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
        std::cerr << "Warning: Failed to set flags on member '" << member.name
                  << "': " << strerror(errno) << "\n";
      }
    }

    // Configure port priority
    if (member.priority) {
      std::memset(&req, 0, sizeof(req));
      std::strncpy(req.ifbr_ifsname, member.name.c_str(), IFNAMSIZ - 1);
      req.ifbr_priority = *member.priority;

      std::memset(&ifd, 0, sizeof(ifd));
      std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
      ifd.ifd_cmd = BRDGSIFPRIO;
      ifd.ifd_len = sizeof(req);
      ifd.ifd_data = &req;

      if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
        std::cerr << "Warning: Failed to set priority on member '" << member.name
                  << "': " << strerror(errno) << "\n";
      }
    }

    // Configure path cost
    if (member.path_cost) {
      std::memset(&req, 0, sizeof(req));
      std::strncpy(req.ifbr_ifsname, member.name.c_str(), IFNAMSIZ - 1);
      req.ifbr_path_cost = *member.path_cost;

      std::memset(&ifd, 0, sizeof(ifd));
      std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
      ifd.ifd_cmd = BRDGSIFCOST;
      ifd.ifd_len = sizeof(req);
      ifd.ifd_data = &req;

      if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
        std::cerr << "Warning: Failed to set path cost on member '" << member.name
                  << "': " << strerror(errno) << "\n";
      }
    }
  }

  // Configure bridge priority
  if (config.priority) {
    struct ifbrparam param;
    std::memset(&param, 0, sizeof(param));
    param.ifbrp_prio = *config.priority;

    struct ifdrv ifd;
    std::memset(&ifd, 0, sizeof(ifd));
    std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
    ifd.ifd_cmd = BRDGSPRI;
    ifd.ifd_len = sizeof(param);
    ifd.ifd_data = &param;

    if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
      std::cerr << "Warning: Failed to set bridge priority: " 
                << strerror(errno) << "\n";
    }
  }

  // Configure hello time
  if (config.hello_time) {
    struct ifbrparam param;
    std::memset(&param, 0, sizeof(param));
    param.ifbrp_hellotime = *config.hello_time;

    struct ifdrv ifd;
    std::memset(&ifd, 0, sizeof(ifd));
    std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
    ifd.ifd_cmd = BRDGSHT;
    ifd.ifd_len = sizeof(param);
    ifd.ifd_data = &param;

    if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
      std::cerr << "Warning: Failed to set hello time: " 
                << strerror(errno) << "\n";
    }
  }

  // Configure forward delay
  if (config.forward_delay) {
    struct ifbrparam param;
    std::memset(&param, 0, sizeof(param));
    param.ifbrp_fwddelay = *config.forward_delay;

    struct ifdrv ifd;
    std::memset(&ifd, 0, sizeof(ifd));
    std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
    ifd.ifd_cmd = BRDGSFD;
    ifd.ifd_len = sizeof(param);
    ifd.ifd_data = &param;

    if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
      std::cerr << "Warning: Failed to set forward delay: " 
                << strerror(errno) << "\n";
    }
  }

  // Configure max age
  if (config.max_age) {
    struct ifbrparam param;
    std::memset(&param, 0, sizeof(param));
    param.ifbrp_maxage = *config.max_age;

    struct ifdrv ifd;
    std::memset(&ifd, 0, sizeof(ifd));
    std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
    ifd.ifd_cmd = BRDGSMA;
    ifd.ifd_len = sizeof(param);
    ifd.ifd_data = &param;

    if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
      std::cerr << "Warning: Failed to set max age: " 
                << strerror(errno) << "\n";
    }
  }

  // Configure aging time
  if (config.aging_time) {
    struct ifbrparam param;
    std::memset(&param, 0, sizeof(param));
    param.ifbrp_ctime = *config.aging_time;

    struct ifdrv ifd;
    std::memset(&ifd, 0, sizeof(ifd));
    std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
    ifd.ifd_cmd = BRDGSTO;
    ifd.ifd_len = sizeof(param);
    ifd.ifd_data = &param;

    if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
      std::cerr << "Warning: Failed to set aging time: " 
                << strerror(errno) << "\n";
    }
  }

  // Configure max addresses
  if (config.max_addresses) {
    struct ifbrparam param;
    std::memset(&param, 0, sizeof(param));
    param.ifbrp_csize = *config.max_addresses;

    struct ifdrv ifd;
    std::memset(&ifd, 0, sizeof(ifd));
    std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
    ifd.ifd_cmd = BRDGSCACHE;
    ifd.ifd_len = sizeof(param);
    ifd.ifd_data = &param;

    if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
      std::cerr << "Warning: Failed to set max addresses: " 
                << strerror(errno) << "\n";
    }
  }

  // Configure STP if requested
  if (config.stp) {
    // Enable STP on member interfaces via BRDGSIFFLGS
    for (const auto &member : config.members) {
      struct ifbreq req;
      std::memset(&req, 0, sizeof(req));
      std::strncpy(req.ifbr_ifsname, member.c_str(), IFNAMSIZ - 1);
      req.ifbr_ifsflags = IFBIF_STP;

      struct ifdrv ifd;
      std::memset(&ifd, 0, sizeof(ifd));
      std::strncpy(ifd.ifd_name, name.c_str(), IFNAMSIZ - 1);
      ifd.ifd_cmd = BRDGSIFFLGS;
      ifd.ifd_len = sizeof(req);
      ifd.ifd_data = &req;

      if (ioctl(sock, SIOCSDRVSPEC, &ifd) < 0) {
        std::cerr << "Warning: Failed to enable STP on member '" << member
                  << "': " << strerror(errno) << "\n";
      }
    }
  }

  close(sock);
}

void SystemConfigurationManager::configureLagg(const std::string &name,
                                                const LaggConfig &config) {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Failed to create socket: " +
                             std::string(strerror(errno)));
  }

  // Set LAGG protocol
  int proto_value = 0;
  switch (config.protocol) {
  case LaggProtocol::FAILOVER:
    proto_value = 1;
    break;
  case LaggProtocol::LOADBALANCE:
    proto_value = 4;
    break;
  case LaggProtocol::LACP:
    proto_value = 3;
    break;
  case LaggProtocol::NONE:
    proto_value = 0;
    break;
  }

  if (proto_value > 0) {
    struct lagg_reqall ra;
    std::memset(&ra, 0, sizeof(ra));
    ra.ra_proto = proto_value;

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
    ifr.ifr_data = reinterpret_cast<char *>(&ra);

    if (ioctl(sock, SIOCSLAGG, &ifr) < 0) {
      close(sock);
      throw std::runtime_error("Failed to set LAGG protocol: " +
                               std::string(strerror(errno)));
    }
  }

  // Add member ports
  for (const auto &member : config.members) {
    struct lagg_reqport rp;
    std::memset(&rp, 0, sizeof(rp));
    std::strncpy(rp.rp_portname, member.c_str(), IFNAMSIZ - 1);

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
    ifr.ifr_data = reinterpret_cast<char *>(&rp);

    if (ioctl(sock, SIOCSLAGGPORT, &ifr) < 0) {
      close(sock);
      throw std::runtime_error("Failed to add port '" + member + "' to LAGG '" +
                               name + "': " + std::string(strerror(errno)));
    }
  }

  // Configure hash policy
  if (config.hash_policy) {
    // Hash policy mapping: layer2, layer2+3, layer3+4
    // This is typically done via sysctl or LAGG-specific ioctls in FreeBSD
    // Note: Specific implementation depends on FreeBSD version
    std::cerr << "Note: Hash policy configuration for LAGG '" << name 
              << "' may require sysctl settings\n";
  }

  // Configure LACP rate
  if (config.lacp_rate) {
    // LACP rate: 0=slow (30s), 1=fast (1s)
    // This is typically configured per-port in FreeBSD
    std::cerr << "Note: LACP rate configuration for LAGG '" << name 
              << "' may require per-port settings\n";
  }

  close(sock);
}

void SystemConfigurationManager::configureVLAN(const std::string &name,
                                                const VLANConfig &config) {
  if (!config.parent || config.id == 0) {
    throw std::runtime_error(
        "VLAN configuration requires parent interface and VLAN ID");
  }

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Failed to create socket: " +
                             std::string(strerror(errno)));
  }

  struct vlanreq vreq;
  std::memset(&vreq, 0, sizeof(vreq));
  std::strncpy(vreq.vlr_parent, config.parent->c_str(), IFNAMSIZ - 1);
  vreq.vlr_tag = config.id;

  // Set PCP (Priority Code Point) if provided
  if (config.pcp) {
    vreq.vlr_tag |= (*config.pcp & 0x7) << 13;  // PCP is bits 13-15
  }

  struct ifreq ifr;
  std::memset(&ifr, 0, sizeof(ifr));
  std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
  ifr.ifr_data = reinterpret_cast<char *>(&vreq);

  if (ioctl(sock, SIOCSETVLAN, &ifr) < 0) {
    close(sock);
    throw std::runtime_error("Failed to configure VLAN: " +
                             std::string(strerror(errno)));
  }

  close(sock);
}

void SystemConfigurationManager::configureGIFTunnel(
    const std::string &name, const std::string &source,
    const std::string &destination,
    const std::optional<int> &tunnel_fib) {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Failed to create socket: " +
                             std::string(strerror(errno)));
  }

  // Parse source and destination addresses
  auto src_addr = IPAddress::fromString(source);
  auto dst_addr = IPAddress::fromString(destination);

  if (!src_addr || !dst_addr) {
    close(sock);
    throw std::runtime_error("Invalid tunnel endpoint addresses");
  }

  if (src_addr->family() != dst_addr->family()) {
    close(sock);
    throw std::runtime_error(
        "Tunnel endpoints must be same address family (both IPv4 or IPv6)");
  }

  // Configure tunnel endpoints using SIOCSIFPHYADDR
  struct ifaliasreq ifra;
  std::memset(&ifra, 0, sizeof(ifra));
  std::strncpy(ifra.ifra_name, name.c_str(), IFNAMSIZ - 1);

  if (src_addr->family() == AddressFamily::IPv4) {
    auto v4src = dynamic_cast<const IPv4Address *>(src_addr.get());
    auto v4dst = dynamic_cast<const IPv4Address *>(dst_addr.get());

    // Source address
    struct sockaddr_in *sin_src =
        reinterpret_cast<struct sockaddr_in *>(&ifra.ifra_addr);
    sin_src->sin_family = AF_INET;
    sin_src->sin_len = sizeof(struct sockaddr_in);
    sin_src->sin_addr.s_addr = htonl(v4src->asUint32());

    // Destination address
    struct sockaddr_in *sin_dst =
        reinterpret_cast<struct sockaddr_in *>(&ifra.ifra_broadaddr);
    sin_dst->sin_family = AF_INET;
    sin_dst->sin_len = sizeof(struct sockaddr_in);
    sin_dst->sin_addr.s_addr = htonl(v4dst->asUint32());

    if (ioctl(sock, SIOCSIFPHYADDR, &ifra) < 0) {
      close(sock);
      throw std::runtime_error("Failed to configure GIF tunnel endpoints: " +
                               std::string(strerror(errno)));
    }
  } else {
    // IPv6 tunnel configuration
    // Note: FreeBSD may require using routing socket for IPv6 tunnel config
    // This is a simplified version - production code may need routing socket approach
    
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
    
    // IPv6 tunnel endpoints typically configured via routing socket messages
    // or specialized ioctl structures. For now, log a warning.
    std::cerr << "Warning: IPv6 tunnel configuration requires routing socket - not fully implemented\n";
  }

  // Set tunnel FIB if specified
  if (tunnel_fib) {
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
    ifr.ifr_fib = *tunnel_fib;

    if (ioctl(sock, SIOCSIFFIB, &ifr) < 0) {
      std::cerr << "Warning: Failed to set tunnel FIB: " << strerror(errno)
                << "\n";
    }
  }

  close(sock);
}
