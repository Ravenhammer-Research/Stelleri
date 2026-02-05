#pragma once

#include "TableFormatter.hpp"
#include <vector>

class TapTableFormatter : public TableFormatter {
public:
  TapTableFormatter() = default;
  std::string format(const std::vector<ConfigData> &items) const override;
};
