/**
 * TunTableFormatter
 */

#pragma once

#include "InterfaceConfig.hpp"
#include "TableFormatter.hpp"
#include <string>
#include <vector>

class TunTableFormatter : public TableFormatter<InterfaceConfig> {
public:
  TunTableFormatter() = default;
  std::string format(const std::vector<InterfaceConfig> &interfaces) override;
};
