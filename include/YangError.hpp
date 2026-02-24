/*
 * YangError.hpp
 * Exception wrapper that carries a YangContext with an error message
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "YangContext.hpp"
#include <format>
#include <libyang/libyang.h>
#include <stdexcept>
#include <string>

class YangError : public std::runtime_error {
public:
  explicit YangError(const YangContext &ctx)
      : std::runtime_error([&]() {
          if (!ctx.get())
            return std::string("yang error");
          const char *msg = ly_errmsg(ctx.get());
          const char *path = ly_errpath(ctx.get());
          if (msg) {
            if (path)
              return std::format("{} ({})", msg, path);
            return std::string(msg);
          }
          return std::string("yang error");
        }()),
        ctx_(ctx) {}

private:
  YangContext ctx_;
};
