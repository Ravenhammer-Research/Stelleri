#include "OvpnTableFormatter.hpp"
#include "InterfaceConfig.hpp"
#include "InterfaceFlags.hpp"
#include "OvpnConfig.hpp"
#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

std::string OvpnTableFormatter::format(const std::vector<InterfaceConfig> &interfaces) {
  if (interfaces.empty()) {
    return "No ovpn interfaces found.\n";
  }

  addColumn("Interface", "Interface", 10, 4, true);
  addColumn("Source", "Source", 5, 6, true);
  addColumn("Destination", "Destination", 5, 6, true);
  addColumn("Flags", "Flags", 5, 3, true);
  addColumn("Metric", "Metric", 4, 3, false);
  addColumn("MTU", "MTU", 4, 3, false);
  addColumn("Groups", "Groups", 2, 6, true);
  addColumn("FIB", "FIB", 5, 3, false);
  addColumn("TunFIB", "TunFIB", 4, 3, false);
  addColumn("ND6Opts", "ND6Opts", 1, 6, true);

  for (const auto &ic : interfaces) {
    std::string source = "-";
    std::string destination = "-";
    std::string flagsStr = "-";
    std::string metricStr = "-";
    std::string mtuStr = "-";
    std::string groupsCell = "-";
    std::string fibStr = "-";
    std::string tunnelFibStr = "-";
    std::string nd6Cell = "-";

    const OvpnConfig *tptr = dynamic_cast<const OvpnConfig *>(&ic);
    if (tptr) {
      if (tptr->source)
        source = tptr->source->toString();
      if (tptr->destination)
        destination = tptr->destination->toString();
    } else {
      continue; // only show OvpnConfig entries
    }

    if (ic.flags)
      flagsStr = flagsToString(*ic.flags);
    if (ic.metric)
      metricStr = std::to_string(*ic.metric);
    if (ic.mtu)
      mtuStr = std::to_string(*ic.mtu);

    if (!ic.groups.empty()) {
      std::ostringstream goss;
      for (size_t i = 0; i < ic.groups.size(); ++i) {
        if (i)
          goss << '\n';
        goss << ic.groups[i];
      }
      groupsCell = goss.str();
    }

    if (ic.vrf) {
      if (ic.vrf->table)
        fibStr = std::to_string(ic.vrf->table);
      else if (ic.vrf->table >= 0)
        fibStr = std::to_string(ic.vrf->table);
    }

    if (tptr && tptr->tunnel_vrf)
      tunnelFibStr = std::to_string(*tptr->tunnel_vrf);

    if (ic.nd6_options)
      nd6Cell = *ic.nd6_options;

    addRow({ic.name, source, destination, flagsStr, metricStr, mtuStr,
            groupsCell, fibStr, tunnelFibStr, nd6Cell});
  }

  auto out = renderTable(80);
  out += "\n";
  return out;
}
