/*
 * StelleriAccessControl.hpp
 * Representation of stelleri-access-control for policy configuration
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "PolicyConfig.hpp"
#include "YangData.hpp"
#include <libyang/libyang.h>
#include <memory>

/**
 * @brief Parser for stelleri-access-control policies
 *
 * Converts between libyang YANG nodes and PolicyConfig objects.
 */
class StelleriAccessControl : public YangData {
public:
  explicit StelleriAccessControl(struct lyd_node *node = nullptr)
      : YangData(node) {}

  // Convert from YANG node to PolicyConfig
  static PolicyConfig fromYang(const struct lyd_node *node);

  // Convert PolicyConfig to YANG node (in the given context)
  struct lyd_node *toLydNode(const YangContext &ctx);

  // Parse a list of policies from a data tree
  static std::vector<PolicyConfig>
  parsePolicies(const struct lyd_node *policies_node);
};
