/**
 * @file RouteTableFormatter.hpp
 * @brief Formatter for route table output
 */

#pragma once

#include "ConfigData.hpp"
#include "TableFormatter.hpp"
#include <string>
#include <vector>

class RouteTableFormatter : public TableFormatter {
public:
  RouteTableFormatter() = default;

  // Format routes as ASCII table
  std::string format(const std::vector<ConfigData> &routes) const override;
};
