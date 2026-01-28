#include "InterfaceTableFormatter.hpp"
#include "InterfaceConfig.hpp"
#include "InterfaceType.hpp"
#include <algorithm>
#include <iomanip>
#include <net/if.h>
#include <sstream>

static std::string flagsToString(uint32_t flags) {
  std::string result;
  if (flags & IFF_UP)
    result += "U";
  if (flags & IFF_BROADCAST)
    result += "B";
  if (flags & IFF_DEBUG)
    result += "D";
  if (flags & IFF_LOOPBACK)
    result += "L";
  if (flags & IFF_POINTOPOINT)
    result += "P";
  if (flags & IFF_RUNNING)
    result += "R";
  if (flags & IFF_NOARP)
    result += "N";
  if (flags & IFF_PROMISC)
    result += "O";
  if (flags & IFF_ALLMULTI)
    result += "A";
  if (flags & IFF_MULTICAST)
    result += "M";

  return result.empty() ? "-" : result;
}

std::string InterfaceTableFormatter::format(
    const std::vector<ConfigData> &interfaces) const {
  if (interfaces.empty()) {
    return "No interfaces found.\n";
  }

  std::ostringstream oss;

  // Calculate column widths
  size_t nameWidth = 9;   // "Interface"
  size_t typeWidth = 4;   // "Type"
  size_t addrWidth = 7;   // "Address"
  size_t statusWidth = 6; // "Status"
  size_t mtuWidth = 3;    // "MTU"
  size_t vrfWidth = 3;    // "VRF"
  size_t flagsWidth = 5;  // "Flags"

  for (const auto &cd : interfaces) {
    if (!cd.iface)
      continue;
    const auto &ic = *cd.iface;
    nameWidth = std::max(nameWidth, ic.name.length());
    typeWidth = std::max(typeWidth, to_string(ic.type).length());
    if (ic.address) {
      addrWidth = std::max(addrWidth, ic.address->toString().length());
    }
    for (const auto &alias : ic.aliases) {
      addrWidth = std::max(addrWidth, alias->toString().length());
    }
    if (ic.mtu) {
      mtuWidth = std::max(mtuWidth, std::to_string(*ic.mtu).length());
    }
    if (ic.vrf) {
      vrfWidth = std::max(vrfWidth, ic.vrf->name.length());
    }
    if (ic.flags) {
      flagsWidth = std::max(flagsWidth, flagsToString(*ic.flags).length());
    }
  }

  // Add padding
  nameWidth += 2;
  typeWidth += 2;
  addrWidth += 2;
  statusWidth += 2;
  mtuWidth += 2;
  vrfWidth += 2;
  flagsWidth += 2;

  // Header
  oss << std::left << std::setw(nameWidth) << "Interface"
      << std::setw(typeWidth) << "Type" << std::setw(addrWidth) << "Address"
      << std::setw(statusWidth) << "Status" << std::setw(mtuWidth) << "MTU"
      << std::setw(vrfWidth) << "VRF" << std::setw(flagsWidth) << "Flags"
      << "\n";

  // Separator
  oss << std::string(nameWidth, '-') << std::string(typeWidth, '-')
      << std::string(addrWidth, '-') << std::string(statusWidth, '-')
      << std::string(mtuWidth, '-') << std::string(vrfWidth, '-')
      << std::string(flagsWidth, '-') << "\n";

  // Rows
  for (const auto &cd : interfaces) {
    if (!cd.iface)
      continue;
    const auto &ic = *cd.iface;

    // Determine link status from RUNNING flag (carrier detected)
    std::string status = "-";
    if (ic.flags) {
      if (*ic.flags & IFF_RUNNING) {
        status = "active";
      } else if (*ic.flags & IFF_UP) {
        status = "no-carrier";
      } else {
        status = "down";
      }
    }

    oss << std::left << std::setw(nameWidth) << ic.name << std::setw(typeWidth)
        << to_string(ic.type);

    // If no primary address but has aliases, show first alias as primary
    if (!ic.address && !ic.aliases.empty()) {
      oss << std::setw(addrWidth) << ic.aliases[0]->toString();
    } else if (ic.address) {
      oss << std::setw(addrWidth) << ic.address->toString();
    } else {
      oss << std::setw(addrWidth) << "-";
    }

    oss << std::setw(statusWidth) << status;

    if (ic.mtu) {
      oss << std::setw(mtuWidth) << *ic.mtu;
    } else {
      oss << std::setw(mtuWidth) << "-";
    }

    if (ic.vrf) {
      oss << std::setw(vrfWidth) << ic.vrf->name;
    } else {
      oss << std::setw(vrfWidth) << "-";
    }

    if (ic.flags) {
      oss << std::setw(flagsWidth) << flagsToString(*ic.flags);
    } else {
      oss << std::setw(flagsWidth) << "-";
    }

    oss << "\n";

    // Print remaining aliases (skip first if we used it as primary)
    size_t startIdx = (!ic.address && !ic.aliases.empty()) ? 1 : 0;
    for (size_t i = startIdx; i < ic.aliases.size(); ++i) {
      oss << std::left << std::setw(nameWidth) << "" << std::setw(typeWidth)
          << "" << std::setw(addrWidth) << ic.aliases[i]->toString()
          << std::setw(statusWidth) << "" << std::setw(mtuWidth) << ""
          << std::setw(vrfWidth) << "" << std::setw(flagsWidth) << ""
          << "\n";
    }
  }

  return oss.str() +
         "\nFlags: U=UP, B=BROADCAST, D=DEBUG, L=LOOPBACK, P=POINTOPOINT, "
         "R=RUNNING, N=NOARP, O=PROMISC, A=ALLMULTI, M=MULTICAST\n";
}
