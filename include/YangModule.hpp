/*
 * YangModule.hpp
 * Lightweight wrapper for a libyang ::lys_module pointer
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include <libyang/libyang.h>
#include <string>

class YangModule {
public:
  explicit YangModule(const struct lys_module *m = nullptr) : mod_(m) {}

  bool valid() const { return mod_ != nullptr; }

  const struct lys_module *getModulePtr() const { return mod_; }

  std::string name() const {
    return mod_ ? std::string(mod_->name) : std::string();
  }

  std::string revision() const {
    if (!mod_)
      return std::string();
    return mod_->revision ? std::string(mod_->revision) : std::string();
  }

private:
  const struct lys_module *mod_ = nullptr;
};
