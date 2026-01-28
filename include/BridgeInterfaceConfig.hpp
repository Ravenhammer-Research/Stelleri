#pragma once

#include <string>
#include <vector>

struct BridgeInterfaceConfig {
  bool stp = false; // spanning tree enabled
  bool vlanFiltering = false;
  std::vector<std::string> members; // member interface names
};
