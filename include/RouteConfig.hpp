#pragma once

#include <optional>
#include <string>

struct RouteConfig {
  std::string prefix;
  std::optional<std::string> nexthop;
  std::optional<std::string> iface;
  std::optional<std::string> vrf;
  bool blackhole = false;
};
