#include "BridgeTableFormatter.hpp"
#include "InterfaceConfig.hpp"
#include "InterfaceType.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

std::string
BridgeTableFormatter::format(const std::vector<ConfigData> &interfaces) const {
  if (interfaces.empty()) {
    return "No bridge interfaces found.\n";
  }

  std::ostringstream oss;
  oss << "\nBridge Interfaces\n";
  oss << std::string(80, '=') << "\n\n";

  for (const auto &cd : interfaces) {
    if (!cd.iface || cd.iface->type != InterfaceType::Bridge)
      continue;

    const auto &ic = *cd.iface;
    oss << "Bridge: " << ic.name << "\n";

    if (ic.bridge) {
      const auto &br = *ic.bridge;

      // Bridge settings
      oss << "  STP:            " << (br.stp ? "enabled" : "disabled") << "\n";
      oss << "  VLAN Filtering: " << (br.vlanFiltering ? "enabled" : "disabled")
          << "\n";

      if (br.priority)
        oss << "  Priority:       " << *br.priority << "\n";
      if (br.hello_time)
        oss << "  Hello Time:     " << *br.hello_time << "s\n";
      if (br.forward_delay)
        oss << "  Forward Delay:  " << *br.forward_delay << "s\n";
      if (br.max_age)
        oss << "  Max Age:        " << *br.max_age << "s\n";
      if (br.aging_time)
        oss << "  Aging Time:     " << *br.aging_time << "s\n";
      if (br.max_addresses)
        oss << "  Max Addresses:  " << *br.max_addresses << "\n";

      // Simple members
      if (!br.members.empty()) {
        oss << "  Members:\n";
        for (const auto &member : br.members) {
          oss << "    - " << member << "\n";
        }
      }

      // Detailed member configurations
      if (!br.member_configs.empty()) {
        oss << "  Member Configurations:\n";
        oss << "    " << std::left << std::setw(12) << "Interface"
            << std::setw(10) << "Priority" << std::setw(12) << "Path Cost"
            << std::setw(8) << "STP" << std::setw(8) << "Edge"
            << "\n";
        oss << "    " << std::string(50, '-') << "\n";

        for (const auto &member : br.member_configs) {
          oss << "    " << std::left << std::setw(12) << member.name
              << std::setw(10)
              << (member.priority ? std::to_string(*member.priority)
                                  : "default")
              << std::setw(12)
              << (member.path_cost ? std::to_string(*member.path_cost) : "auto")
              << std::setw(8) << (member.stp ? "yes" : "no") << std::setw(8)
              << (member.edge ? "yes" : "no") << "\n";
        }
      }
    }

    if (ic.address) {
      oss << "  Address:        " << ic.address->toString() << "\n";
    }
    if (ic.mtu) {
      oss << "  MTU:            " << *ic.mtu << "\n";
    }

    oss << "\n";
  }

  return oss.str();
}
