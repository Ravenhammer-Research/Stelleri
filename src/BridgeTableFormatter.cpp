#include "BridgeTableFormatter.hpp"
#include "BridgeInterfaceConfig.hpp"
#include "InterfaceConfig.hpp"
#include "InterfaceFlags.hpp"
#include "InterfaceType.hpp"
#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <regex>
#include <sstream>
#include <vector>

std::string
BridgeTableFormatter::format(const std::vector<ConfigData> &interfaces) const {
  if (interfaces.empty()) {
    return "No bridge interfaces found.\n";
  }
  // Produce a compact, tabular bridge summary similar to other formatters.
  std::ostringstream oss;
  oss << std::left << std::setw(15) << "Interface" << std::setw(8) << "STP"
      << std::setw(15) << "VLANFiltering" << std::setw(10) << "Priority"
      << std::setw(20) << "Members" << std::setw(8) << "MTU" << std::setw(8)
      << "Flags" << "\n";
  oss << std::string(100, '-') << "\n";

  for (const auto &cd : interfaces) {
    if (!cd.iface || cd.iface->type != InterfaceType::Bridge)
      continue;

    const auto &ic = *cd.iface;
    const auto *br =
        dynamic_cast<const BridgeInterfaceConfig *>(cd.iface.get());

    std::string stp = (br && br->stp) ? "yes" : "no";
    std::string vlanf = (br && br->vlanFiltering) ? "yes" : "no";
    std::string prio =
        (br && br->priority) ? std::to_string(*br->priority) : std::string("-");
    std::vector<std::string> memberList;
    if (br && !br->members.empty()) {
      for (const auto &m : br->members)
        memberList.push_back(m);
    }
    std::string mtu = ic.mtu ? std::to_string(*ic.mtu) : std::string("-");
    std::string flags =
        (ic.flags ? flagsToString(*ic.flags) : std::string("-"));

    if (memberList.empty()) {
      oss << std::left << std::setw(15) << ic.name << std::setw(8) << stp
          << std::setw(15) << vlanf << std::setw(10) << prio << std::setw(20)
          << "-" << std::setw(8) << mtu << std::setw(8) << flags << "\n";
    } else {
      // First member on the primary row
      oss << std::left << std::setw(15) << ic.name << std::setw(8) << stp
          << std::setw(15) << vlanf << std::setw(10) << prio << std::setw(20)
          << memberList[0] << std::setw(8) << mtu << std::setw(8) << flags
          << "\n";
      // Remaining members on indented subsequent rows
      for (size_t i = 1; i < memberList.size(); ++i) {
        oss << std::left << std::setw(15) << "" << std::setw(8) << ""
            << std::setw(15) << "" << std::setw(10) << "" << std::setw(20)
            << memberList[i] << std::setw(8) << "" << std::setw(8) << ""
            << "\n";
      }
    }
  }

  return oss.str();
}
