/* CommitToken.cpp -- parse and execute `set commit` */

#if defined(STELLERI_NETCONF) && STELLERI_NETCONF == 1

#include "CommitToken.hpp"
#include "Client.hpp"
#include "NetconfExecutor.hpp"
#include <iostream>

std::shared_ptr<CommitToken>
CommitToken::parseFromTokens(const std::vector<std::string> &tokens,
                             size_t start, size_t &next) {
  // start points at "commit"
  size_t cur = start;
  if (cur >= tokens.size() || tokens[cur] != "commit")
    return nullptr;
  ++cur;
  next = cur;
  return std::make_shared<CommitToken>();
}

void CommitToken::executeSet(ConfigurationManager *) const {
  auto c = Client::instance();
  if (!c) {
    std::cerr << "commit: no client session configured\n";
    return;
  }
  auto reply = NetconfExecutor::commit(c->session());
  if (!reply) {
    std::cerr << "commit: failed (no reply)\n";
    return;
  }
  if (reply->isOk())
    std::cout << "commit: ok\n";
  else
    std::cout << "commit: failed\n";
}

#endif
