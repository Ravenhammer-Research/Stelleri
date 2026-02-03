/**
 * @file TableFormatter.hpp
 * @brief Abstract base for table formatters
 */

#pragma once

#include "ConfigData.hpp"
#include <string>
#include <vector>

class TableFormatter {
public:
  TableFormatter() = default;
  virtual ~TableFormatter() = default;

  // Format a list of ConfigData as an ASCII table
  virtual std::string format(const std::vector<ConfigData> &data) const = 0;
};
