#pragma once

#include "ConfigData.hpp"
#include <string>
#include <vector>

class InterfaceTableFormatter {
public:
  InterfaceTableFormatter() = default;

  // Format a list of interface ConfigData into an ASCII table
  std::string format(const std::vector<ConfigData> &interfaces) const;
};
