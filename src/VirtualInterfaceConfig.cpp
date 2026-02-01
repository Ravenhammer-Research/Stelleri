#include "VirtualInterfaceConfig.hpp"

#include <iostream>

VirtualInterfaceConfig::VirtualInterfaceConfig(const InterfaceConfig &base) {
  name = base.name;
  type = base.type;
  if (base.address) address = base.address->clone();
  aliases.clear();
  for (const auto &a : base.aliases) {
    if (a)
      aliases.push_back(a->clone());
  }
  if (base.vrf) vrf = std::make_unique<VRFConfig>(*base.vrf);
  flags = base.flags;
  tunnel_vrf = base.tunnel_vrf;
  groups = base.groups;
  mtu = base.mtu;
}

VirtualInterfaceConfig::VirtualInterfaceConfig(const InterfaceConfig &base,
                                               std::optional<std::string> peer_,
                                               std::optional<int> rdomain_,
                                               bool promiscuous_)
{
  name = base.name;
  type = base.type;
  if (base.address) address = base.address->clone();
  aliases.clear();
  for (const auto &a : base.aliases) {
    if (a)
      aliases.push_back(a->clone());
  }
  if (base.vrf) vrf = std::make_unique<VRFConfig>(*base.vrf);
  flags = base.flags;
  tunnel_vrf = base.tunnel_vrf;
  groups = base.groups;
  mtu = base.mtu;

  peer = peer_;
  rdomain = rdomain_;
  promiscuous = promiscuous_;
}

void VirtualInterfaceConfig::save() const {
  if (!ConfigData::exists(name)) {
    create();
  } else {
    InterfaceConfig::save();
  }

  if (promiscuous) {
    std::cerr << "Note: enabling promiscuous mode for '" << name << "' may require additional privileges or steps\n";
  }
}

void VirtualInterfaceConfig::create() const {
  const std::string &nm = name;
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
  }

  struct ifreq ifr;
  std::memset(&ifr, 0, sizeof(ifr));
  std::strncpy(ifr.ifr_name, nm.c_str(), IFNAMSIZ - 1);

  if (ioctl(sock, SIOCIFCREATE, &ifr) < 0) {
    close(sock);
    throw std::runtime_error("Failed to create interface '" + nm + "': " + std::string(strerror(errno)));
  }

  close(sock);
}
