/**
 * TunTableFormatter
 */

#pragma once

#include "TunInterfaceConfig.hpp"
#include "TableFormatter.hpp"
#include <string>
#include <vector>

class TunTableFormatter : public TableFormatter<TunInterfaceConfig> {
public:
  TunTableFormatter() = default;
  std::string format(const std::vector<TunInterfaceConfig> &interfaces) override;
};
