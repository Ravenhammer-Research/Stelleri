/**
 * IpsecTableFormatter
 */

#pragma once

#include "InterfaceConfig.hpp"
#include "TableFormatter.hpp"
#include <string>
#include <vector>

class IpsecTableFormatter : public TableFormatter<InterfaceConfig> {
public:
  IpsecTableFormatter() = default;
  std::string format(const std::vector<InterfaceConfig> &interfaces) override;
};
