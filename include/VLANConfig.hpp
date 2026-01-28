#pragma once

#include <cstdint>
#include <optional>
#include <string>

struct VLANConfig {
  uint16_t id = 0;
  std::optional<std::string> name;
  // parent interface name when applicable (e.g., "em0" for em0.100)
  std::optional<std::string> parent;
};
