/*
 * IetfInterfaces.hpp
 * Representation of ietf-interfaces `interface` list as C++ properties
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "IanaInterfaceType.hpp"
#include "InterfaceConfig.hpp"
#include "InterfaceFlags.hpp"
#include "InterfaceType.hpp"
#include "Session.hpp"
#include "SystemConfigurationManager.hpp"
#include "YangData.hpp"
#include <arpa/inet.h>
#include <charconv>
#include <format>
#include <libyang/libyang.h>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class IetfInterfaces : public YangData {
public:
  explicit IetfInterfaces(struct lyd_node *node = nullptr) : YangData(node) {}

  ~IetfInterfaces() override = default;
  InterfaceConfig iface;
  std::optional<std::string> bind_ni_name;

  std::vector<std::unique_ptr<IetfInterfaces>>
  getIetfInterfaces(const YangData &data [[maybe_unused]],
                    const YangContext &ctx [[maybe_unused]],
                    const Session &session [[maybe_unused]]) {
    std::vector<std::unique_ptr<IetfInterfaces>> out;
    SystemConfigurationManager scm;
    auto ifs = scm.GetInterfaces();

    for (const auto &ic : ifs) {
      auto obj = std::make_unique<IetfInterfaces>(ic, nullptr);
      out.push_back(std::move(obj));
    }

    return out;
  }

  explicit IetfInterfaces(InterfaceConfig ic, struct lyd_node *node = nullptr)
      : YangData(node), iface(std::move(ic)) {}

  static constexpr const char iface_fmt[] =
      "ietf-interfaces:interfaces/interface[name='{}']";
  static constexpr const char leaf_fmt[] =
      "ietf-interfaces:interfaces/interface[name='{}']/{}";
  static constexpr const char ipv4_mtu_fmt[] =
      "ietf-interfaces:interfaces/interface[name='{}']/ip:ipv4/ip:mtu";
  static constexpr const char ipv6_mtu_fmt[] =
      "ietf-interfaces:interfaces/interface[name='{}']/ip:ipv6/ip:mtu";
  static constexpr const char ipv4_addr_prefix_fmt[] =
      "ietf-interfaces:interfaces/interface[name='{}']/ip:ipv4/"
      "ip:address[ip='{}']/ip:prefix-length";
  static constexpr const char ipv4_addr_netmask_fmt[] =
      "ietf-interfaces:interfaces/interface[name='{}']/ip:ipv4/"
      "ip:address[ip='{}']/ip:netmask";
  static constexpr const char ipv6_addr_prefix_fmt[] =
      "ietf-interfaces:interfaces/interface[name='{}']/ip:ipv6/"
      "ip:address[ip='{}']/ip:prefix-length";
  static constexpr const char ipv6_addr_origin_fmt[] =
      "ietf-interfaces:interfaces/interface[name='{}']/ip:ipv6/"
      "ip:address[ip='{}']/ip:origin";

  struct lyd_node *toLydNode(const YangContext &ctx) {
    if (!ctx.get())
      return nullptr;

    std::string iface_path = std::format(iface_fmt, iface.name);

    newPath(ctx, iface_path.c_str(), "");

    if (iface.description)
      newPath(ctx, leaf_fmt, *iface.description, iface.name, "description");

    if (bind_ni_name) {
      newPath(ctx, leaf_fmt, *bind_ni_name, iface.name, "bind-ni-name");
    }

    std::string iname = netconf::interfaceTypeToIanaIdentity(iface.type);
    newPath(ctx, leaf_fmt, iname, iface.name, "type");

    std::string enabled = "true";
    if (iface.flags && !hasFlag(*iface.flags, InterfaceFlag::UP))
      enabled = "false";

    newPath(ctx, leaf_fmt, enabled, iface.name, "enabled");

    if (iface.index)
      newPath(ctx, leaf_fmt, std::to_string(*iface.index), iface.name,
              "if-index");

    if (iface.hwaddr)
      newPath(ctx, leaf_fmt, *iface.hwaddr, iface.name, "phys-address");

    if (iface.baudrate)
      newPath(ctx, leaf_fmt, std::to_string(*iface.baudrate), iface.name,
              "speed");

    if (iface.mtu) {
      std::string mtu_str = std::to_string(*iface.mtu);
      newPath(ctx, ipv4_mtu_fmt, mtu_str, iface.name);
      newPath(ctx, ipv6_mtu_fmt, mtu_str, iface.name);
    }

    auto emitNetwork = [&](const IPNetwork *net) {
      if (!net)
        return;
      auto addrPtr = net->address();
      if (!addrPtr)
        return;
      std::string ipstr = addrPtr->toString();
      std::string plen = std::to_string(net->mask());
      if (net->family() == AddressFamily::IPv4) {
        std::string cidr_mask;

        if (auto m = IPAddress::maskFromCIDR(AddressFamily::IPv4, net->mask()))
          cidr_mask = m->toString();

        std::string subnet_mask;
        if (auto sm = net->subnet())
          subnet_mask = sm->toString();

        if (!subnet_mask.empty() && !cidr_mask.empty() &&
            subnet_mask != cidr_mask) {
          newPath(ctx, ipv4_addr_netmask_fmt, subnet_mask, iface.name, ipstr);
        } else {
          newPath(ctx, ipv4_addr_prefix_fmt, plen, iface.name, ipstr);
        }
      } else if (net->family() == AddressFamily::IPv6) {
        newPath(ctx, ipv6_addr_prefix_fmt, plen, iface.name, ipstr);

        if (const IPv6Network *v6 = dynamic_cast<const IPv6Network *>(net)) {
          if (v6->pltime || v6->vltime) {
            newPath(ctx, ipv6_addr_origin_fmt, "random", iface.name, ipstr);
          } else {
            if (const IPv6Address *ip6 =
                    dynamic_cast<const IPv6Address *>(addrPtr.get())) {
              unsigned __int128 val = ip6->value();
              uint16_t top = static_cast<uint16_t>((val >> 112) & 0xffff);
              if (top == 0xfe80u)
                newPath(ctx, ipv6_addr_origin_fmt, "link-layer", iface.name,
                        ipstr);
              else
                newPath(ctx, ipv6_addr_origin_fmt, "static", iface.name, ipstr);
            } else {
              newPath(ctx, ipv6_addr_origin_fmt, "static", iface.name, ipstr);
            }
          }
        } else {
          if (const IPv6Address *ip6 =
                  dynamic_cast<const IPv6Address *>(addrPtr.get())) {
            unsigned __int128 val = ip6->value();
            uint16_t top = static_cast<uint16_t>((val >> 112) & 0xffff);
            if (top == 0xfe80u)
              newPath(ctx, ipv6_addr_origin_fmt, "link-layer", iface.name,
                      ipstr);
            else
              newPath(ctx, ipv6_addr_origin_fmt, "static", iface.name, ipstr);
          } else {
            newPath(ctx, ipv6_addr_origin_fmt, "static", iface.name, ipstr);
          }
        }
      }
    };

    // Primary address
    if (iface.address)
      emitNetwork(iface.address.get());

    // Aliases
    for (const auto &a : iface.aliases) {
      if (a)
        emitNetwork(a.get());
    }

    return node_;
  }

  static std::unique_ptr<YangData>
  toTree(const std::vector<std::unique_ptr<IetfInterfaces>> &list,
         const YangContext &ctx) {
    if (!ctx.get())
      return nullptr;
    struct lyd_node *first_created = nullptr;
    for (const auto &it : list) {
      struct lyd_node *n = it->toLydNode(ctx);
      if (!n)
        return nullptr;
      if (!first_created)
        first_created = n;
    }
    if (!first_created)
      return nullptr;
    // Walk up to the top-level `interfaces` container using libyang accessor
    struct lyd_node *root = first_created;
    while (lyd_parent(root))
      root = lyd_parent(root);
    return std::make_unique<YangData>(root);
  }

  // Convert the currently held libyang `interface` node into an
  // InterfaceConfig structure. If no node is present an empty
  // InterfaceConfig is returned.
  InterfaceConfig toInterfaceConfig() const {
    InterfaceConfig out;
    if (!node_)
      return out;

    bool havePrimaryAddr = false;

    struct lyd_node *match = nullptr;
    // name
    if (lyd_find_path(node_, "name", 0, &match) == LY_SUCCESS && match) {
      const char *v = lyd_get_value(match);
      if (v)
        out.name = v;
    }
    // description
    match = nullptr;
    if (lyd_find_path(node_, "description", 0, &match) == LY_SUCCESS && match) {
      const char *v = lyd_get_value(match);
      if (v)
        out.description = v;
    }
    // type
    match = nullptr;
    if (lyd_find_path(node_, "type", 0, &match) == LY_SUCCESS && match) {
      const char *v = lyd_get_value(match);
      if (v)
        out.type = netconf::ianaIdentityToInterfaceType(std::string(v));
    }
    // enabled
    match = nullptr;
    if (lyd_find_path(node_, "enabled", 0, &match) == LY_SUCCESS && match) {
      const char *v = lyd_get_value(match);
      if (v && std::string(v) == "true")
        out.flags = static_cast<uint32_t>(InterfaceFlag::UP);
    }
    // if-index
    match = nullptr;
    if (lyd_find_path(node_, "if-index", 0, &match) == LY_SUCCESS && match) {
      const char *v = lyd_get_value(match);
      if (v && *v) {
        int iv = 0;
        auto res = std::from_chars(v, v + std::strlen(v), iv);
        if (res.ec == std::errc())
          out.index = iv;
      }
    }
    // phys-address
    match = nullptr;
    if (lyd_find_path(node_, "phys-address", 0, &match) == LY_SUCCESS &&
        match) {
      const char *v = lyd_get_value(match);
      if (v)
        out.hwaddr = v;
    }
    // speed
    match = nullptr;
    if (lyd_find_path(node_, "speed", 0, &match) == LY_SUCCESS && match) {
      const char *v = lyd_get_value(match);
      if (v && *v) {
        unsigned long long uv = 0;
        auto res = std::from_chars(v, v + std::strlen(v), uv);
        if (res.ec == std::errc())
          out.baudrate = static_cast<uint64_t>(uv);
      }
    }
    // mtu
    match = nullptr;
    if (lyd_find_path(node_, "mtu", 0, &match) == LY_SUCCESS && match) {
      const char *v = lyd_get_value(match);
      if (v && *v) {
        int iv = 0;
        auto res = std::from_chars(v, v + std::strlen(v), iv);
        if (res.ec == std::errc())
          out.mtu = iv;
      }
    }

    // addresses (address child nodes)
    match = nullptr;
    if (lyd_find_path(node_, "address", 0, &match) == LY_SUCCESS && match) {
      // iterate through address instances under this interface
      struct lyd_node *addr = match;
      for (; addr; addr = addr->next) {
        // get children ip / prefix-length / netmask
        struct lyd_node *ipn = nullptr;
        if (lyd_find_path(addr, "ip", 0, &ipn) == LY_SUCCESS && ipn) {
          const char *ipstr = lyd_get_value(ipn);
          std::string plen;
          struct lyd_node *pln = nullptr;
          if (lyd_find_path(addr, "prefix-length", 0, &pln) == LY_SUCCESS &&
              pln) {
            const char *pv = lyd_get_value(pln);
            if (pv)
              plen = pv;
          }
          struct lyd_node *nm = nullptr;
          if (lyd_find_path(addr, "netmask", 0, &nm) == LY_SUCCESS && nm) {
            const char *nv = lyd_get_value(nm);
            if (nv) {
              // convert netmask to prefix
              struct in_addr m4;
              std::string full;
              if (inet_pton(AF_INET, nv, &m4) == 1) {
                uint32_t mv = ntohl(m4.s_addr);
                int bits = 0;
                for (int i = 31; i >= 0; --i) {
                  if ((mv >> i) & 1u)
                    ++bits;
                  else
                    break;
                }
                full = std::string(ipstr ? ipstr : "") + "/" +
                       std::to_string(bits);
              } else {
                full = std::string(ipstr ? ipstr : "");
              }
              if (auto net = IPNetwork::fromString(full)) {
                if (!havePrimaryAddr) {
                  out.address = std::move(net);
                  havePrimaryAddr = true;
                } else {
                  out.aliases.push_back(std::move(net));
                }
              }
              continue;
            }
          }
          if (ipstr && *ipstr) {
            std::string full = ipstr;
            if (!plen.empty())
              full += "/" + plen;
            if (auto net = IPNetwork::fromString(full)) {
              if (!havePrimaryAddr) {
                out.address = std::move(net);
                havePrimaryAddr = true;
              } else {
                out.aliases.push_back(std::move(net));
              }
            }
          }
        }
        if (!addr->next)
          break;
      }
    }

    return out;
  }
};