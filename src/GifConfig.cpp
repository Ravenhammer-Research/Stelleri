#include "GifConfig.hpp"
#include "ConfigurationManager.hpp"
#include "InterfaceType.hpp"

void GifConfig::save(ConfigurationManager &mgr) const { mgr.SaveGif(*this); }

void GifConfig::create(ConfigurationManager &mgr) const { mgr.CreateGif(name); }

GifConfig::GifConfig(const InterfaceConfig &base) {
  name = base.name;
  type = InterfaceType::Gif;
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

GifConfig::GifConfig(const InterfaceConfig &base,
                     std::unique_ptr<IPAddress> source,
                     std::unique_ptr<IPAddress> destination)
    : GifConfig(base) {
  this->source = std::move(source);
  this->destination = std::move(destination);
}
