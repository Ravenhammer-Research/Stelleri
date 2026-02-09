#include "GenerateIpsecCommands.hpp"
#include "InterfaceToken.hpp"
#include "IpsecConfig.hpp"
#include <iostream>

namespace netcli {

void generateIpsecCommands(ConfigurationManager &mgr,
                           std::set<std::string> &processedInterfaces) {
  auto ipsecs = mgr.GetIpsecInterfaces();
  for (const auto &ifc : ipsecs) {
    if (processedInterfaces.count(ifc.name))
      continue;
    std::cout << std::string("set ") + InterfaceToken::toString(
                     const_cast<IpsecConfig *>(&ifc))
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
