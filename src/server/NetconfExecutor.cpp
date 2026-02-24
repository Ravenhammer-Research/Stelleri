/*
 * NetconfExecutor.cpp
 * Provide static `NetconfExecutor::*` method definitions matching the
 * declarations in include/NetconfExecutor.hpp.
 */

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "NetconfExecutor.hpp"
#include "DataStore.hpp"
#include "NetconfEditConfigOperation.hpp"
#include "NetconfServerReply.hpp"
#include "Session.hpp"
#include "YangContext.hpp"
#include "YangData.hpp"
#include <memory>

std::unique_ptr<NetconfServerReply> NetconfExecutor::get(const Session &session
                                                         [[maybe_unused]],
                                                         const YangData &filter
                                                         [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::getConfig(const Session &session [[maybe_unused]],
                           const YangData &filter [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::editConfig(const Session &session [[maybe_unused]],
                            const YangData &target [[maybe_unused]],
                            NetconfEditConfigOperation op [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::copyConfig(const Session &session [[maybe_unused]],
                            DataStore src [[maybe_unused]],
                            DataStore dst [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::deleteConfig(const Session &session [[maybe_unused]],
                              const YangData &target [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::commit(const Session &session [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply> NetconfExecutor::lock(const Session &session
                                                          [[maybe_unused]],
                                                          DataStore ds
                                                          [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::unlock(const Session &session [[maybe_unused]],
                        DataStore ds [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::closeSession(const Session &session [[maybe_unused]],
                              const Session &target [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::killSession(const Session &session [[maybe_unused]],
                             const Session &target [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::validate(const Session &session [[maybe_unused]],
                          const YangData &target [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::discardChanges(const Session &session [[maybe_unused]]) {
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}
