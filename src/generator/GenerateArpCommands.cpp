/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 */

#include "GenerateArpCommands.hpp"
#include "ArpConfig.hpp"
#include "ArpToken.hpp"
#include <iostream>

namespace netcli {

  void generateArpCommands(ConfigurationManager &mgr) {
    auto entries = mgr.GetArpEntries();
    for (auto &entry : entries) {
      // Only emit permanent (static) entries — dynamic ones are learned at
      // runtime and should not be persisted in configuration.
      if (!entry.permanent)
        continue;
      std::cout << "set " << ArpToken::toString(&entry) << "\n";
    }
  }

} // namespace netcli
