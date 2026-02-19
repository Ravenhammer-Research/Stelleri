/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 */

#include "GenerateNdpCommands.hpp"
#include "NdpConfig.hpp"
#include "NdpToken.hpp"
#include <iostream>

namespace netcli {

  void generateNdpCommands(ConfigurationManager &mgr) {
    auto entries = mgr.GetNdpEntries();
    for (auto &entry : entries) {
      // Only emit permanent (static) entries — dynamic ones are learned at
      // runtime and should not be persisted in configuration.
      if (!entry.permanent)
        continue;
      std::cout << "set " << NdpToken::toString(&entry) << "\n";
    }
  }

} // namespace netcli
