#include "VirtualTableFormatter.hpp"
#include "InterfaceConfig.hpp"
#include "InterfaceFlags.hpp"
#include "InterfaceType.hpp"
#include "VirtualInterfaceConfig.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

std::string
VirtualTableFormatter::format(const std::vector<ConfigData> &interfaces) const {
  if (interfaces.empty()) {
    return "No virtual interfaces found.\n";
  }

  std::ostringstream oss;
  oss << "\nVirtual Interfaces\n";
  oss << std::string(80, '=') << "\n\n";

  // Table header
  oss << std::left << std::setw(15) << "Interface" << std::setw(15) << "Peer"
      << std::setw(20) << "Address" << std::setw(8) << "MTU" << std::setw(10)
      << "RDomain" << std::setw(8) << "Flags"
      << "\n";
  oss << std::string(76, '-') << "\n";

  for (const auto &cd : interfaces) {
    if (!cd.iface || cd.iface->type != InterfaceType::Virtual)
      continue;

    const auto &ic = *cd.iface;

    std::string peer = "-";
    std::string rdomain = "-";

    if (ic.virtual_iface) {
      if (ic.virtual_iface->peer)
        peer = *ic.virtual_iface->peer;
      if (ic.virtual_iface->rdomain)
        rdomain = std::to_string(*ic.virtual_iface->rdomain);
    }

    std::string flags = "-";
    if (ic.flags)
      flags = flagsToString(*ic.flags);

    oss << std::left << std::setw(15) << ic.name << std::setw(15) << peer
        << std::setw(20) << (ic.address ? ic.address->toString() : "-")
        << std::setw(8) << (ic.mtu ? std::to_string(*ic.mtu) : "-")
        << std::setw(10) << rdomain << std::setw(8) << flags << "\n";
  }

  oss << "\n";
  return oss.str();
}
