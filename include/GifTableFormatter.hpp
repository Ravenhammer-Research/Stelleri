/**
 * GifTableFormatter
 */

#pragma once

#include "InterfaceConfig.hpp"
#include "TableFormatter.hpp"
#include <string>
#include <vector>

class GifTableFormatter : public TableFormatter<InterfaceConfig> {
public:
  GifTableFormatter() = default;
  std::string format(const std::vector<InterfaceConfig> &interfaces) override;
};
