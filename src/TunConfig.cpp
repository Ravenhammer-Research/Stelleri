#include "TunConfig.hpp"
#include "ConfigurationManager.hpp"
#include "InterfaceType.hpp"

void TunConfig::save(ConfigurationManager &mgr) const { mgr.SaveTun(*this); }

void TunConfig::create(ConfigurationManager &mgr) const { mgr.CreateTun(name); }

TunConfig::TunConfig(const InterfaceConfig &base) {
  name = base.name;
  type = InterfaceType::Tun;
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

TunConfig::TunConfig(const InterfaceConfig &base,
                     std::unique_ptr<IPAddress> source,
                     std::unique_ptr<IPAddress> destination)
    : TunConfig(base) {
  this->source = std::move(source);
  this->destination = std::move(destination);
}
