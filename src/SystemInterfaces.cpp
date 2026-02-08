/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "InterfaceConfig.hpp"
#include "SystemConfigurationManager.hpp"
#include "WlanAuthMode.hpp"
#include "WlanConfig.hpp"

#include <cstring>
#include <iostream>
#include <ifaddrs.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/if.h>
#include <net80211/ieee80211_ioctl.h>
#include <netinet/in.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <unistd.h>
#include <unordered_map>

void SystemConfigurationManager::prepare_ifreq(struct ifreq &ifr,
                                               const std::string &name) const {
  std::memset(&ifr, 0, sizeof(ifr));
  std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
}

std::optional<int> SystemConfigurationManager::query_ifreq_int(
    const std::string &ifname, unsigned long req, IfreqIntField which) const {
  int s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s < 0)
    return std::nullopt;
  struct ifreq ifr;
  prepare_ifreq(ifr, ifname);
  if (ioctl(s, req, &ifr) == 0) {
    int v = 0;
    switch (which) {
    case IfreqIntField::Metric:
      v = ifr.ifr_metric;
      break;
    case IfreqIntField::Fib:
      v = ifr.ifr_fib;
      break;
    case IfreqIntField::Mtu:
      v = ifr.ifr_mtu;
      break;
    }
    close(s);
    return v;
  }
  close(s);
  return std::nullopt;
}

std::optional<std::pair<std::string, int>>
SystemConfigurationManager::query_ifreq_sockaddr(const std::string &ifname,
                                                 unsigned long req) const {
  int s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s < 0)
    return std::nullopt;
  struct ifreq ifr;
  prepare_ifreq(ifr, ifname);
  if (ioctl(s, req, &ifr) == 0) {
    if (ifr.ifr_addr.sa_family == AF_INET) {
      struct sockaddr_in *sin =
          reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr);
      char buf[INET_ADDRSTRLEN] = {0};
      if (inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf))) {
        close(s);
        return std::make_pair(std::string(buf), AF_INET);
      }
    } else if (ifr.ifr_addr.sa_family == AF_INET6) {
      struct sockaddr_in6 *sin6 =
          reinterpret_cast<struct sockaddr_in6 *>(&ifr.ifr_addr);
      char buf[INET6_ADDRSTRLEN] = {0};
      if (inet_ntop(AF_INET6, &sin6->sin6_addr, buf, sizeof(buf))) {
        close(s);
        return std::make_pair(std::string(buf), AF_INET6);
      }
    }
  }
  close(s);
  return std::nullopt;
}

std::vector<std::string> SystemConfigurationManager::query_interface_groups(
    const std::string &ifname) const {
  std::vector<std::string> out;
  int s = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if (s < 0)
    return out;
  struct ifgroupreq ifgr;
  std::memset(&ifgr, 0, sizeof(ifgr));
  std::strncpy(ifgr.ifgr_name, ifname.c_str(), IFNAMSIZ - 1);
  if (ioctl(s, SIOCGIFGROUP, &ifgr) == 0) {
    size_t len = ifgr.ifgr_len;
    if (len > 0) {
      size_t count = len / sizeof(struct ifg_req);
      std::vector<struct ifg_req> groups(count);
      ifgr.ifgr_groups = groups.data();
      if (ioctl(s, SIOCGIFGROUP, &ifgr) == 0) {
        for (size_t i = 0; i < count; ++i) {
          std::string gname(groups[i].ifgrq_group);
          if (!gname.empty())
            out.emplace_back(gname);
        }
      }
    }
  }
  close(s);
  return out;
}

std::optional<std::string> SystemConfigurationManager::query_ifstatus_nd6(
    const std::string &ifname) const {
  (void)ifname;
  return std::nullopt;
}

// Helper: detect interface type from a struct ifaddrs (platform-specific)
static InterfaceType detectInterfaceTypeFromIfa(const struct ifaddrs *ifa) {
  if (!ifa)
    return InterfaceType::Unknown;
  unsigned int flags = ifa->ifa_flags;
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
    case IFT_IEEE8023ADLAG:
      return InterfaceType::Lagg;
    case IFT_LOOP:
      return InterfaceType::Loopback;
    case IFT_PPP:
      return InterfaceType::PPP;
    case IFT_TUNNEL:
      return InterfaceType::Tunnel;
    case IFT_GIF:
      return InterfaceType::Gif;
    case IFT_FDDI:
      return InterfaceType::FDDI;
    case IFT_ISO88025:
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
  if (flags & IFF_LOOPBACK)
    return InterfaceType::Loopback;
  if (flags & IFF_POINTOPOINT)
    return InterfaceType::PointToPoint;
  return InterfaceType::Unknown;
}

// Helper: build an IPNetwork from ifaddrs address/netmask (returns nullptr if not IPv4/6)
static std::unique_ptr<IPNetwork> ipNetworkFromIfa(const struct ifaddrs *ifa) {
  if (!ifa || !ifa->ifa_addr)
    return nullptr;
  char buf[INET6_ADDRSTRLEN] = {0};
  if (ifa->ifa_addr->sa_family == AF_INET) {
    struct sockaddr_in *a = reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr);
    if (inet_ntop(AF_INET, &a->sin_addr, buf, sizeof(buf))) {
      uint8_t masklen = 32;
      if (ifa->ifa_netmask)
        masklen = IPNetwork::masklenFromSockaddr(ifa->ifa_netmask);
      return IPNetwork::fromString(std::string(buf) + "/" + std::to_string(masklen));
    }
  } else if (ifa->ifa_addr->sa_family == AF_INET6) {
    struct sockaddr_in6 *a6 = reinterpret_cast<struct sockaddr_in6 *>(ifa->ifa_addr);
    if (inet_ntop(AF_INET6, &a6->sin6_addr, buf, sizeof(buf))) {
      uint8_t masklen = 128;
      if (ifa->ifa_netmask)
        masklen = IPNetwork::masklenFromSockaddr(ifa->ifa_netmask);
      return IPNetwork::fromString(std::string(buf) + "/" + std::to_string(masklen));
    }
  }
  return nullptr;
}

void SystemConfigurationManager::populateInterfaceMetadata(
    InterfaceConfig &ic) const {
  if (auto m = query_ifreq_int(ic.name, SIOCGIFMETRIC, IfreqIntField::Metric);
      m) {
    ic.metric = *m;
  }
  if (auto f = query_ifreq_int(ic.name, SIOCGIFFIB, IfreqIntField::Fib); f) {
    if (!ic.vrf)
      ic.vrf = std::make_unique<VRFConfig>(*f);
    else
      ic.vrf->table = *f;
  }
  unsigned int ifidx = if_nametoindex(ic.name.c_str());
  if (ifidx != 0) {
    ic.index = static_cast<int>(ifidx);
  }

  auto groups = query_interface_groups(ic.name);
  for (const auto &g : groups)
    ic.groups.emplace_back(g);
  if (auto nd6 = query_ifstatus_nd6(ic.name); nd6)
    ic.nd6_options = *nd6;

  if (ic.type == InterfaceType::Wireless) {
    if (auto w = dynamic_cast<WlanConfig *>(&ic)) {
      int s = socket(AF_INET, SOCK_DGRAM, 0);
      if (s >= 0) {
        struct ieee80211req req;
        std::memset(&req, 0, sizeof(req));
        std::strncpy(req.i_name, ic.name.c_str(), IFNAMSIZ - 1);

        {
          const int bufsize = 256;
          std::vector<char> buf(bufsize);
          req.i_type = IEEE80211_IOC_SSID;
          req.i_len = bufsize;
          req.i_data = buf.data();
          if (ioctl(s, SIOCG80211, &req) == 0 && req.i_len > 0) {
            w->ssid = std::string(buf.data(), static_cast<size_t>(req.i_len));
          }
        }

        {
          int ch = 0;
          req.i_type = IEEE80211_IOC_CURCHAN;
          req.i_data = &ch;
          req.i_len = sizeof(ch);
          if (ioctl(s, SIOCG80211, &req) == 0) {
            w->channel = ch;
          } else {
            ch = 0;
            req.i_type = IEEE80211_IOC_CHANNEL;
            req.i_data = &ch;
            req.i_len = sizeof(ch);
            if (ioctl(s, SIOCG80211, &req) == 0)
              w->channel = ch;
          }
        }

        {
          const int macsz = 8;
          std::vector<unsigned char> mac(macsz);
          req.i_type = IEEE80211_IOC_BSSID;
          req.i_len = macsz;
          req.i_data = mac.data();
          if (ioctl(s, SIOCG80211, &req) == 0 && req.i_len >= 6) {
            char macbuf[32];
            std::snprintf(macbuf, sizeof(macbuf),
                          "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1],
                          mac[2], mac[3], mac[4], mac[5]);
            w->bssid = std::string(macbuf);
          }
        }

        {
          const int psize = 64;
          std::vector<char> pbuf(psize);
          req.i_type = IEEE80211_IOC_IC_NAME;
          req.i_len = psize;
          req.i_data = pbuf.data();
          if (ioctl(s, SIOCG80211, &req) == 0 && req.i_len > 0)
            w->parent =
                std::string(pbuf.data(), static_cast<size_t>(req.i_len));
        }

        {
          int am = 0;
          req.i_type = IEEE80211_IOC_AUTHMODE;
          req.i_data = &am;
          req.i_len = sizeof(am);
          if (ioctl(s, SIOCG80211, &req) == 0) {
            WlanAuthMode m = WlanAuthMode::UNKNOWN;
            switch (am) {
            case 0:
              m = WlanAuthMode::OPEN;
              break;
            case 1:
              m = WlanAuthMode::SHARED;
              break;
            case 2:
              m = WlanAuthMode::WPA;
              break;
            case 3:
              m = WlanAuthMode::WPA2;
              break;
            default:
              m = WlanAuthMode::UNKNOWN;
              break;
            }
            w->authmode = m;
          }
        }

        if (ic.flags && (*ic.flags & IFF_RUNNING)) {
          if (w->ssid)
            w->status = std::string("associated");
          else
            w->status = std::string("up");
        } else {
          w->status = std::string("down");
        }
        if (w->channel) {
          w->media =
              std::string("IEEE 802.11 channel ") + std::to_string(*w->channel);
        }

        close(s);
      }
    }
  }
}

void SystemConfigurationManager::SaveInterface(const InterfaceConfig &ic) const {
  if (ic.name.empty())
    throw std::runtime_error("Interface has no name");

  if (!InterfaceConfig::exists(ic.name))
    CreateInterface(ic.name);

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Failed to create socket: " +
                             std::string(strerror(errno)));
  }

  // Configure addresses (primary + aliases) if provided
  if (ic.address || !ic.aliases.empty()) {
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, ic.name.c_str(), IFNAMSIZ - 1);

    // Configure primary IPv4 address if requested
    if (ic.address && ic.address->family() == AddressFamily::IPv4) {
      auto v4 = dynamic_cast<IPv4Network *>(ic.address.get());
      if (v4) {
        auto netAddr = v4->address();
        auto v4addr = dynamic_cast<IPv4Address *>(netAddr.get());
        if (v4addr) {
          struct sockaddr_in sa;
          std::memset(&sa, 0, sizeof(sa));
          sa.sin_len = sizeof(sa);
          sa.sin_family = AF_INET;
          sa.sin_addr.s_addr = htonl(v4addr->value());

          struct ifreq aifr;
          std::memset(&aifr, 0, sizeof(aifr));
          std::strncpy(aifr.ifr_name, ic.name.c_str(), IFNAMSIZ - 1);
          std::memcpy(&aifr.ifr_addr, &sa, sizeof(sa));
          if (ioctl(sock, SIOCSIFADDR, &aifr) < 0) {
            std::cerr << "Warning: SIOCSIFADDR failed for " << ic.name << ": "
                      << strerror(errno) << "\n";
          }
        }
      }
    }

    // Configure IPv4 aliases
    for (const auto &aptr : ic.aliases) {
      if (!aptr)
        continue;
      if (aptr->family() != AddressFamily::IPv4)
        continue;
      auto a4net = dynamic_cast<IPv4Network *>(aptr.get());
      if (!a4net)
        continue;

      struct ifaliasreq iar;
      std::memset(&iar, 0, sizeof(iar));
      std::strncpy(iar.ifra_name, ic.name.c_str(), IFNAMSIZ - 1);

      struct sockaddr_in sa;
      std::memset(&sa, 0, sizeof(sa));
      sa.sin_len = sizeof(sa);
      sa.sin_family = AF_INET;
      auto netAddr = a4net->address();
      auto v4addr = dynamic_cast<IPv4Address *>(netAddr.get());
      if (!v4addr)
        continue;
      sa.sin_addr.s_addr = htonl(v4addr->value());
      std::memcpy(&iar.ifra_addr, &sa, sizeof(sa));

      uint32_t maskval = (a4net->mask() == 0) ? 0u : (~0u << (32 - a4net->mask()));
      struct sockaddr_in mask;
      std::memset(&mask, 0, sizeof(mask));
      mask.sin_len = sizeof(mask);
      mask.sin_family = AF_INET;
      mask.sin_addr.s_addr = htonl(maskval);
      std::memcpy(&iar.ifra_mask, &mask, sizeof(mask));

      uint32_t host = v4addr->value();
      uint32_t netn = host & maskval;
      uint32_t bcast = netn | (~maskval);
      struct sockaddr_in broad;
      std::memset(&broad, 0, sizeof(broad));
      broad.sin_len = sizeof(broad);
      broad.sin_family = AF_INET;
      broad.sin_addr.s_addr = htonl(bcast);
      std::memcpy(&iar.ifra_broadaddr, &broad, sizeof(broad));

      if (ioctl(sock, SIOCAIFADDR, &iar) < 0) {
        // fallback to set addr only
        struct ifreq rifr;
        std::memset(&rifr, 0, sizeof(rifr));
        std::strncpy(rifr.ifr_name, ic.name.c_str(), IFNAMSIZ - 1);
        std::memcpy(&rifr.ifr_addr, &sa, sizeof(sa));
        if (ioctl(sock, SIOCSIFADDR, &rifr) < 0) {
          std::cerr << "Warning: SIOCSIFADDR failed when adding alias to " << ic.name
                    << ": " << strerror(errno) << "\n";
          continue;
        }
      }
    }
  }

  // Configure MTU
  if (ic.mtu) {
    struct ifreq ifr_mtu_req;
    std::memset(&ifr_mtu_req, 0, sizeof(ifr_mtu_req));
    std::strncpy(ifr_mtu_req.ifr_name, ic.name.c_str(), IFNAMSIZ - 1);
    ifr_mtu_req.ifr_mtu = *ic.mtu;
    if (ioctl(sock, SIOCSIFMTU, &ifr_mtu_req) < 0) {
      std::cerr << "Warning: Failed to set MTU on " << ic.name << ": "
                << strerror(errno) << "\n";
    }
  }

  // Bring interface up / configure flags
  struct ifreq ifr_flags_req;
  std::memset(&ifr_flags_req, 0, sizeof(ifr_flags_req));
  std::strncpy(ifr_flags_req.ifr_name, ic.name.c_str(), IFNAMSIZ - 1);
  if (ioctl(sock, SIOCGIFFLAGS, &ifr_flags_req) >= 0) {
    ifr_flags_req.ifr_flags |= IFF_UP;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr_flags_req) < 0) {
      std::cerr << "Warning: Failed to bring interface up: " << strerror(errno)
                << "\n";
    }
  }

  // Apply VRF / FIB to the interface if requested
  if (ic.vrf && ic.vrf->table) {
    struct ifreq fib_ifr;
    std::memset(&fib_ifr, 0, sizeof(fib_ifr));
    std::strncpy(fib_ifr.ifr_name, ic.name.c_str(), IFNAMSIZ - 1);
    fib_ifr.ifr_fib = ic.vrf->table;
    if (ioctl(sock, SIOCSIFFIB, &fib_ifr) < 0) {
      int err = errno;
      close(sock);
      throw std::runtime_error("Failed to set interface FIB: " +
                               std::string(strerror(err)));
    }
  }

  close(sock);
}

bool SystemConfigurationManager::InterfaceExists(std::string_view name) const {
  bool found = false;
  struct ifaddrs *ifs = nullptr;
  if (getifaddrs(&ifs) == 0) {
    for (struct ifaddrs *ifa = ifs; ifa; ifa = ifa->ifa_next) {
      if (ifa->ifa_name && name == ifa->ifa_name) {
        found = true;
        break;
      }
    }
    freeifaddrs(ifs);
  }
  return found;
}

std::vector<std::string> SystemConfigurationManager::GetInterfaceAddresses(
    const std::string &ifname, int family) const {
  std::vector<std::string> out;
  struct ifaddrs *ifap = nullptr;
  if (getifaddrs(&ifap) == 0) {
    for (struct ifaddrs *ifa = ifap; ifa; ifa = ifa->ifa_next) {
      if (!ifa->ifa_name)
        continue;
      if (ifname != ifa->ifa_name)
        continue;
      if (!ifa->ifa_addr)
        continue;
      if (family == AF_INET && ifa->ifa_addr->sa_family == AF_INET) {
        char buf[INET_ADDRSTRLEN] = {0};
        struct sockaddr_in *sin = reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr);
        if (inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf))) {
          out.emplace_back(std::string(buf) + "/32");
        }
      } else if (family == AF_INET6 && ifa->ifa_addr->sa_family == AF_INET6) {
        char buf[INET6_ADDRSTRLEN] = {0};
        struct sockaddr_in6 *sin6 = reinterpret_cast<struct sockaddr_in6 *>(ifa->ifa_addr);
        if (inet_ntop(AF_INET6, &sin6->sin6_addr, buf, sizeof(buf))) {
          out.emplace_back(std::string(buf) + "/128");
        }
      }
    }
    freeifaddrs(ifap);
  }
  return out;
}

void SystemConfigurationManager::DestroyInterface(const std::string &name) const {
  if (name.empty())
    throw std::runtime_error("InterfaceConfig::destroy(): empty interface name");

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Failed to create socket: " +
                             std::string(strerror(errno)));
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

void SystemConfigurationManager::RemoveInterfaceAddress(const std::string &ifname, const std::string &addr) const {
  // Parse addr CIDR and remove single address via SIOCDIFADDR for IPv4
  auto net = IPNetwork::fromString(addr);
  if (!net)
    throw std::runtime_error("Invalid address: " + addr);

  if (net->family() == AddressFamily::IPv4) {
    auto v4 = dynamic_cast<IPv4Network *>(net.get());
    if (!v4)
      throw std::runtime_error("Invalid IPv4 address object");

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
      throw std::runtime_error(std::string("failed to create socket: ") + strerror(errno));

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);
    struct sockaddr_in *sin = reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr);
    sin->sin_family = AF_INET;
    struct in_addr a4;
    auto ipaddr = v4->address();
    auto ipv4addr = dynamic_cast<IPv4Address *>(ipaddr.get());
    if (!ipv4addr) {
      close(sock);
      throw std::runtime_error("Invalid IPv4 address object");
    }
    a4.s_addr = htonl(ipv4addr->value());
    sin->sin_addr = a4;
    if (ioctl(sock, SIOCDIFADDR, &ifr) < 0) {
      int err = errno;
      close(sock);
      throw std::runtime_error(std::string("failed to remove address: ") + strerror(err));
    }
    close(sock);
    return;
  }

  // IPv6 removal: not fully implemented here
  throw std::runtime_error("IPv6 address removal not implemented: " + addr);
}

void SystemConfigurationManager::CreateInterface(const std::string &name) const {
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
    throw std::runtime_error("Failed to create interface '" + name + "': " + std::string(strerror(errno)));
  }

  close(sock);
}

std::vector<InterfaceConfig> SystemConfigurationManager::GetInterfaces(
    const std::optional<VRFConfig> &vrf) const {
  std::vector<InterfaceConfig> out;
  struct ifaddrs *ifs = nullptr;
  if (getifaddrs(&ifs) != 0)
    return out;

  std::unordered_map<std::string, std::shared_ptr<InterfaceConfig>> map;
  for (struct ifaddrs *ifa = ifs; ifa; ifa = ifa->ifa_next) {
    if (!ifa->ifa_name)
      continue;
    std::string name = ifa->ifa_name;
    auto it = map.find(name);
    if (it == map.end()) {
      // Build primary InterfaceConfig from ifa fields (system-level parsing)
      InterfaceType t = detectInterfaceTypeFromIfa(ifa);
      std::unique_ptr<IPNetwork> addr = ipNetworkFromIfa(ifa);
      std::vector<std::unique_ptr<IPNetwork>> aliases;
      std::optional<uint32_t> flags = std::nullopt;
      if (ifa->ifa_flags)
        flags = ifa->ifa_flags;
      auto ic = std::shared_ptr<InterfaceConfig>(new InterfaceConfig(
          name, t, std::move(addr), std::move(aliases), nullptr, flags, {}, std::nullopt));
      if (ic->type == InterfaceType::Wireless) {
        auto w = std::shared_ptr<WlanConfig>(new WlanConfig(*ic));
        map.emplace(name, std::move(w));
      } else {
        map.emplace(name, std::move(ic));
      }
    } else {
      auto existing = it->second;
      if (ifa->ifa_addr) {
        if (ifa->ifa_addr->sa_family == AF_INET ||
            ifa->ifa_addr->sa_family == AF_INET6) {
          auto tmpaddr = ipNetworkFromIfa(ifa);
          if (tmpaddr) {
            if (!existing->address) {
              existing->address = std::move(tmpaddr);
            } else {
              existing->aliases.emplace_back(std::move(tmpaddr));
            }
          }
        }
      }
    }
  }

  for (auto &kv : map) {
    populateInterfaceMetadata(*kv.second);
    for (const auto &g : kv.second->groups) {
      if (g == "epair") {
        kv.second->type = InterfaceType::Virtual;
        break;
      }
    }
    if (interfaceIsLagg(kv.second->name)) {
      kv.second->type = InterfaceType::Lagg;
    } else if (interfaceIsBridge(kv.second->name)) {
      kv.second->type = InterfaceType::Bridge;
    }
    if (matches_vrf(*kv.second, vrf))
      out.emplace_back(std::move(*kv.second));
  }

  freeifaddrs(ifs);
  return out;
}

std::vector<InterfaceConfig> SystemConfigurationManager::GetInterfacesByGroup(
    const std::optional<VRFConfig> &vrf, std::string_view group) const {
  auto bases = GetInterfaces(vrf);
  std::vector<InterfaceConfig> out;
  for (const auto &ic : bases) {
    for (const auto &g : ic.groups) {
      if (g == group) {
        out.push_back(ic);
        break;
      }
    }
  }
  return out;
}
