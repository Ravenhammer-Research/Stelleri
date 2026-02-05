#pragma once

#include "TableFormatter.hpp"
#include <vector>

class SixToFourTableFormatter : public TableFormatter {
public:
  SixToFourTableFormatter() = default;
  std::string format(const std::vector<ConfigData> &items) const override;
};
