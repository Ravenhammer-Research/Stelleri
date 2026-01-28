#pragma once

#include <optional>
#include <string>

struct VRFConfig {
  std::string name;
  std::optional<int> table;
  bool bind_to_all = false;
};
