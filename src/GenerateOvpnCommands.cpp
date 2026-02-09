#include "GenerateOvpnCommands.hpp"
#include "InterfaceToken.hpp"
#include "OvpnConfig.hpp"
#include <iostream>

namespace netcli {

void generateOvpnCommands(ConfigurationManager &mgr,
                           std::set<std::string> &processedInterfaces) {
  auto ovpns = mgr.GetOvpnInterfaces();
  for (const auto &ifc : ovpns) {
    if (processedInterfaces.count(ifc.name))
      continue;
    std::cout << std::string("set ") + InterfaceToken::toString(
                     const_cast<OvpnConfig *>(&ifc))
              << "\n";
    processedInterfaces.insert(ifc.name);
    for (const auto &alias : ifc.aliases) {
      InterfaceConfig tmp = ifc;
      tmp.address = alias->clone();
      std::cout << std::string("set ") + InterfaceToken::toString(&tmp)
                << "\n";
    }
  }
}

} // namespace netcli
