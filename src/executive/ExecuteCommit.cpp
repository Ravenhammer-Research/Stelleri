/* ExecuteCommit.cpp
 * Handler for 'set commit' command
 */

#if defined(STELLERI_NETCONF) && STELLERI_NETCONF == 1

#include "CommitToken.hpp"
#include "ConfigurationManager.hpp"

namespace netcli {

  void executeCommit(const CommitToken &tok, ConfigurationManager *mgr) {
    tok.executeSet(mgr);
  }

} // namespace netcli

#endif
