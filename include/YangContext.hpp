/*
 * YangContext.hpp
 * Lightweight wrapper for a libyang `ly_ctx` pointer.
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

// Use the installed libyang header from /usr/local/include/libyang
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#include <libyang/libyang.h>
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#pragma GCC diagnostic ignored "-Wnested-anon-types"
#include <libyang/libyang.h>
#pragma GCC diagnostic pop
#else
#include <libyang/libyang.h>
#endif

class YangContext {
public:
  // Non-owning wrapper around an existing libyang context.
  explicit YangContext(struct ly_ctx *ctx = nullptr) : ctx_(ctx) {}
  ~YangContext() = default;

  // Access the underlying libyang context pointer.
  struct ly_ctx *get() const { return ctx_; }

private:
  struct ly_ctx *ctx_ = nullptr;
};
