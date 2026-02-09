#include "OvpnConfig.hpp"
#include "ConfigurationManager.hpp"
#include "InterfaceType.hpp"

void OvpnConfig::save(ConfigurationManager &mgr) const { mgr.SaveOvpn(*this); }

void OvpnConfig::create(ConfigurationManager &mgr) const { mgr.CreateOvpn(name); }

OvpnConfig::OvpnConfig(const InterfaceConfig &base) {
  name = base.name;
  type = InterfaceType::Tunnel;
  if (base.address)
    address = base.address->clone();
  aliases.clear();
  for (const auto &a : base.aliases) {
    if (a)
      aliases.emplace_back(a->clone());
    else
      aliases.emplace_back(nullptr);
  }
  if (base.vrf)
    vrf = std::make_unique<VRFConfig>(*base.vrf);
  flags = base.flags;
  groups = base.groups;
  mtu = base.mtu;
}

OvpnConfig::OvpnConfig(const InterfaceConfig &base,
                       std::unique_ptr<IPAddress> source,
                       std::unique_ptr<IPAddress> destination)
    : OvpnConfig(base) {
  this->source = std::move(source);
  this->destination = std::move(destination);
}
