#pragma once

#include "TableFormatter.hpp"
#include <vector>

class CarpTableFormatter : public TableFormatter {
public:
  CarpTableFormatter() = default;
  std::string format(const std::vector<ConfigData> &items) const override;
};
