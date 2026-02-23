/*
 * YangData.hpp
 * Abstract representation of YANG-modeled data
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include <memory>
#include <string>

#include <format>

#include <cstdlib>
#include <functional>

#include "FilterType.hpp"
#include "YangContext.hpp"
#include "YangDataTypes.hpp"
#include <cstring>
#include <libyang/libyang.h>

// Create a safe local enum backed by constexpr values that mirror libyang's
// macros. This lets the rest of the code use a typed enum while still
// compiling even if libyang's macros are defined as tricky tokens.
#ifdef LY_SUCCESS
static constexpr int LY_SUCCESS_VAL = LY_SUCCESS;
#else
static constexpr int LY_SUCCESS_VAL = 0;
#endif

#ifdef LY_ERR
static constexpr int LY_ERR_VAL = LY_ERR;
#else
static constexpr int LY_ERR_VAL = -1;
#endif

enum class LyResult : int { Success = LY_SUCCESS_VAL, Error = LY_ERR_VAL };

class YangData {
public:
  virtual ~YangData() {
    if (node_)
      lyd_free_all(node_);
  }

  // Accept an optional libyang data tree node pointer.
  explicit YangData(struct lyd_node *node = nullptr)
      : node_(node), filter_(SubTree) {}

  // Alternate constructor accepting a specific filter type.
  YangData(struct lyd_node *node, FilterType filter)
      : node_(node), filter_(filter) {}

  virtual std::unique_ptr<YangData> clone() const {
    if (!node_)
      return std::make_unique<YangData>(nullptr);
    struct lyd_node *dup = nullptr;
    int rc = lyd_dup_single(node_, nullptr, 0, &dup);
    if (rc != static_cast<int>(LyResult::Success) || !dup)
      return nullptr;
    return std::make_unique<YangData>(dup);
  }

  [[deprecated("toXML is deprecated; prefer toLydNode() and letting "
               "libnetconf2 handle serialization on its own")]]
  virtual std::string toXML() const {
    if (!node_)
      return std::string();
    char *buf = nullptr;
    LY_ERR rc = lyd_print_mem(&buf, node_, LYD_XML, 0);
    if (rc != LY_SUCCESS || !buf)
      return std::string();
    std::string s(buf);
    free(buf);
    return s;
  }

  struct lyd_node *toLydNode() const { return node_; }

  std::string ns() const {
    if (!node_)
      return std::string();
    const struct lysc_node *schema = node_->schema;
    if (!schema || !schema->module)
      return std::string();
    const char *n = schema->module->ns;
    return n ? std::string(n) : std::string();
  }

  // Unified variadic helper: first variadic argument is treated as the
  // node value; any remaining args are used to format the path. The
  // value is stringified via `std::format("{}", value)` so callers
  // may pass string-like or numeric types.
  template <typename V, typename... Args>
  struct lyd_node *newPath(const YangContext &ctx, const char *fmt, V &&value,
                           Args &&...args) {
    if (!ctx.get())
      return nullptr;

    std::string path = std::format(fmt, std::forward<Args>(args)...);
    std::string v = std::format("{}", std::forward<V>(value));

    /* libyang lyd_new_path signature expects parent first on this system */
    struct lyd_node *out = nullptr;
    int rc = lyd_new_path(node_, ctx.get(), path.c_str(), v.c_str(), 0, &out);
    if (rc != static_cast<int>(LyResult::Success))
      return nullptr;
    node_ = out;
    return node_;
  }

protected:
  struct lyd_node *node_ = nullptr;
  FilterType filter_ = None;
  bool hasFilter() const { return filter_ != None; }
};
