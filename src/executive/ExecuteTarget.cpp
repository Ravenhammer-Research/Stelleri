/* ExecuteTarget.cpp
 * Handler for 'set target' command
 */

#if defined(STELLERI_NETCONF) && STELLERI_NETCONF == 1

#include "ConfigurationManager.hpp"
#include "TargetToken.hpp"

namespace netcli {

  void executeTarget(const TargetToken &tok, ConfigurationManager *) {
    tok.executeSet(nullptr);
  }

} // namespace netcli

#endif
