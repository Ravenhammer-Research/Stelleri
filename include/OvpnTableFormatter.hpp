/**
 * OvpnTableFormatter
 */

#pragma once

#include "InterfaceConfig.hpp"
#include "TableFormatter.hpp"
#include <string>
#include <vector>

class OvpnTableFormatter : public TableFormatter<InterfaceConfig> {
public:
  OvpnTableFormatter() = default;
  std::string format(const std::vector<InterfaceConfig> &interfaces) override;
};
