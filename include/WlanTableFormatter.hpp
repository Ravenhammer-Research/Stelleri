#pragma once

#include "TableFormatter.hpp"
#include <vector>

class WlanTableFormatter : public TableFormatter {
public:
  WlanTableFormatter() = default;
  std::string format(const std::vector<ConfigData> &items) const override;
};
