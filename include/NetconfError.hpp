/*
 * NetconfError.hpp
 * Exception wrapper that carries a Client pointer and a YangContext
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "YangContext.hpp"
#include <stdexcept>
#include <string>

class NetconfError : public std::runtime_error {
public:
  NetconfError(Client *client, const YangContext &ctx)
      : std::runtime_error("netconf error"), client_(client), ctx_(ctx) {}

  Client *client() const noexcept { return client_; }
  const YangContext &context() const noexcept { return ctx_; }

private:
  Client *client_ = nullptr; // non-owning
  YangContext ctx_;
};
