#include "VLANTableFormatter.hpp"
#include "InterfaceConfig.hpp"
#include "InterfaceType.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

std::string
VLANTableFormatter::format(const std::vector<ConfigData> &interfaces) const {
  if (interfaces.empty()) {
    return "No VLAN interfaces found.\n";
  }

  std::ostringstream oss;
  oss << "\nVLAN Interfaces\n";
  oss << std::string(80, '=') << "\n\n";

  // Table header
  oss << std::left << std::setw(15) << "Interface" << std::setw(10) << "VLAN ID"
      << std::setw(15) << "Parent" << std::setw(8) << "PCP" << std::setw(20)
      << "Address" << std::setw(8) << "MTU"
      << "\n";
  oss << std::string(76, '-') << "\n";

  for (const auto &cd : interfaces) {
    if (!cd.iface || cd.iface->type != InterfaceType::VLAN)
      continue;

    const auto &ic = *cd.iface;

    if (!ic.vlan)
      continue;

    const auto &vlan = *ic.vlan;

    oss << std::left << std::setw(15) << ic.name << std::setw(10) << vlan.id
        << std::setw(15) << (vlan.parent ? *vlan.parent : "-") << std::setw(8)
        << (vlan.pcp ? std::to_string(*vlan.pcp) : "-") << std::setw(20)
        << (ic.address ? ic.address->toString() : "-") << std::setw(8)
        << (ic.mtu ? std::to_string(*ic.mtu) : "-") << "\n";
  }

  oss << "\n";
  return oss.str();
}
