#include "BridgeInterfaceConfig.hpp"
#include "ConfigurationManager.hpp"
#include "InterfaceConfig.hpp"
#include "InterfaceToken.hpp"
#include "LaggConfig.hpp"
#include "LoopBackConfig.hpp"
#include "Parser.hpp"
#include "TunnelConfig.hpp"
#include "VLANConfig.hpp"
#include "VirtualInterfaceConfig.hpp"
#include <iostream>

void netcli::Parser::executeSetInterface(const InterfaceToken &tok,
                                         ConfigurationManager *mgr) const {
  const std::string name = tok.name();
  if (name.empty()) {
    std::cerr << "set interface: missing interface name\n";
    return;
  }

  auto itype = tok.type();
  try {
    // Load existing configuration if present (so we can update), otherwise
    // prepare a new base for creation.
    bool exists = InterfaceConfig::exists(name);
    InterfaceConfig base;
    if (exists && mgr) {
      auto cdopt = mgr->getInterface(name);
      if (cdopt && cdopt->iface)
        base = *cdopt->iface;
      else
        base.name = name;
    } else {
      base.name = name;
    }

    // If the token requested a specific VRF/FIB table, apply it to the base
    if (tok.vrf) {
      try {
        int tbl = std::stoi(*tok.vrf);
        if (!base.vrf)
          base.vrf = std::make_unique<VRFConfig>();
        base.vrf->table = tbl;
        base.vrf->name = std::string("fib") + std::to_string(tbl);
      } catch (...) {
        // ignore invalid fib value
      }
    }

    // Decide effective type: token > existing > name prefix
    InterfaceType effectiveType = InterfaceType::Unknown;
    if (itype != InterfaceType::Unknown)
      effectiveType = itype;
    else if (base.type != InterfaceType::Unknown)
      effectiveType = base.type;
    else if (base.name.rfind("lo", 0) == 0)
      effectiveType = InterfaceType::Loopback;

    // Handle concrete types
    if (effectiveType == InterfaceType::Bridge) {
      BridgeInterfaceConfig bic(base);
      bic.save();
      std::cout << "set interface: " << (exists ? "updated" : "created")
                << " bridge '" << name << "'\n";
      return;
    }

    if (effectiveType == InterfaceType::Lagg) {
      if (!tok.lagg || tok.lagg->members.empty()) {
        std::cerr << "set interface: LAGG creation typically requires member "
                     "interfaces.\n"
                  << "Usage: set interface name <lagg_name> lagg members "
                     "<if1,if2,...> [protocol <proto>]\n";
        return;
      }
      LaggConfig lac(base, tok.lagg->protocol, tok.lagg->members,
                     tok.lagg->hash_policy, tok.lagg->lacp_rate,
                     tok.lagg->min_links);
      lac.save();
      std::cout << "set interface: " << (exists ? "updated" : "created")
                << " lagg '" << name << "'\n";
      return;
    }

    if (effectiveType == InterfaceType::VLAN) {
      if (!tok.vlan || tok.vlan->id == 0 || !tok.vlan->parent) {
        std::cerr << "set interface: VLAN creation requires VLAN id and parent "
                     "interface.\n"
                  << "Usage: set interface name <vlan_name> vlan id <vlan_id> "
                     "parent <parent_iface>\n";
        return;
      }
      VLANConfig vc(base, tok.vlan->id, tok.vlan->name, tok.vlan->parent,
                    tok.vlan->pcp);
      vc.InterfaceConfig::name = name;
      vc.save();
      std::cout << "set interface: " << (exists ? "updated" : "created")
                << " vlan '" << name << "'\n";
      return;
    }

    if (effectiveType == InterfaceType::Tunnel ||
        effectiveType == InterfaceType::Gif ||
        effectiveType == InterfaceType::Tun) {
      TunnelConfig tc(base);
      tc.save();
      std::cout << "set interface: " << (exists ? "updated" : "created")
                << " tunnel '" << name << "'\n";
      return;
    }

    if (effectiveType == InterfaceType::Loopback) {
      LoopBackConfig lbc(base);
      lbc.save();
      std::cout << "set interface: " << (exists ? "updated" : "created")
                << " loopback '" << name << "'\n";
      return;
    }

    if (effectiveType == InterfaceType::Virtual) {
      VirtualInterfaceConfig vic(base);
      vic.save();
      std::cout << "set interface: " << (exists ? "updated" : "created")
                << " virtual iface '" << name << "'\n";
      return;
    }

    // Default: apply generic interface updates
    base.save();
    std::cout << "set interface: " << (exists ? "updated" : "created")
              << " interface '" << name << "'\n";
  } catch (const std::exception &e) {
    std::cerr << "set interface: failed to create/update '" << name
              << "': " << e.what() << "\n";
  }
}
