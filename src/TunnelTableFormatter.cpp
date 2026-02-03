#include "TunnelTableFormatter.hpp"
#include "InterfaceConfig.hpp"
#include "InterfaceType.hpp"
#include "TunnelConfig.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

std::string
TunnelTableFormatter::format(const std::vector<ConfigData> &interfaces) const {
  if (interfaces.empty()) {
    return "No tunnel interfaces found.\n";
  }

  std::ostringstream oss;
  oss << "\nTunnel Interfaces\n";
  oss << std::string(80, '=') << "\n\n";

  // Table header
  oss << std::left << std::setw(12) << "Interface" << std::setw(18) << "Source"
      << std::setw(18) << "Destination" << std::setw(10) << "Tunnel VRF"
      << std::setw(8) << "TTL" << std::setw(8) << "TOS"
      << "\n";
  oss << std::string(74, '-') << "\n";

  for (const auto &cd : interfaces) {
    if (!cd.iface || cd.iface->type != InterfaceType::Tunnel)
      continue;

    const auto &ic = *cd.iface;

    std::string source = "-";
    std::string destination = "-";
    std::string ttl = "-";
    std::string tos = "-";

    if (ic.tunnel) {
      if (ic.tunnel->source)
        source = ic.tunnel->source->toString();
      if (ic.tunnel->destination)
        destination = ic.tunnel->destination->toString();
      if (ic.tunnel->ttl)
        ttl = std::to_string(*ic.tunnel->ttl);
      if (ic.tunnel->tos)
        tos = std::to_string(*ic.tunnel->tos);
    }

    std::string tunnel_vrf = "-";
    if (ic.tunnel_vrf)
      tunnel_vrf = std::to_string(*ic.tunnel_vrf);

    oss << std::left << std::setw(12) << ic.name << std::setw(18) << source
        << std::setw(18) << destination << std::setw(10) << tunnel_vrf
        << std::setw(8) << ttl << std::setw(8) << tos << "\n";
  }

  oss << "\n";
  return oss.str();
}
