#pragma once

#include <string>
#include <vector>

enum class LaggProtocol { LACP, FAILOVER, LOADBALANCE, NONE };

struct LaggConfig {
  LaggProtocol protocol = LaggProtocol::NONE;
  std::vector<std::string> members; // member interface names
};
