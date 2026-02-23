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

std::unique_ptr<NetconfServerReply>
NetconfExecutor::get(const Session &session, const YangData &filter) {
  (void)session;
  (void)filter;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::getConfig(const Session &session, const YangData &filter) {
  (void)session;
  (void)filter;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::editConfig(const Session &session, const YangData &target,
                            NetconfEditConfigOperation op) {
  (void)session;
  (void)target;
  (void)op;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::copyConfig(const Session &session, DataStore src,
                            DataStore dst) {
  (void)session;
  (void)src;
  (void)dst;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::deleteConfig(const Session &session, const YangData &target) {
  (void)session;
  (void)target;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::commit(const Session &session) {
  (void)session;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::lock(const Session &session, DataStore ds) {
  (void)session;
  (void)ds;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::unlock(const Session &session, DataStore ds) {
  (void)session;
  (void)ds;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::closeSession(const Session &session, const Session &target) {
  (void)session;
  (void)target;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::killSession(const Session &session, const Session &target) {
  (void)session;
  (void)target;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::validate(const Session &session, const YangData &target) {
  (void)session;
  (void)target;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}

std::unique_ptr<NetconfServerReply>
NetconfExecutor::discardChanges(const Session &session) {
  (void)session;
  auto r = std::make_unique<NetconfServerReply>(NetconfServerReply::RPL_ERROR);
  r->setError(NetconfServerReply::ERR_OP_NOT_SUPPORTED);
  return r;
}
