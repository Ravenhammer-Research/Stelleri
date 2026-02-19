/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 */

#pragma once

#include "ConfigurationManager.hpp"
#include <set>
#include <string>

namespace netcli {
  void generateWireGuardCommands(ConfigurationManager &mgr,
                                 std::set<std::string> &processedInterfaces);
} // namespace netcli
