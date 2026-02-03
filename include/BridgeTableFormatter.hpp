/**
 * @file BridgeTableFormatter.hpp
 * @brief Formatter for bridge interface details
 */

#pragma once

#include "BridgeConfig.hpp"
#include "ConfigData.hpp"
#include "TableFormatter.hpp"
#include <string>
#include <vector>

/**
 * @brief Formats bridge interface configuration as ASCII table
 *
 * Shows bridge-specific details like members, STP settings, priorities, timers.
 */
class BridgeTableFormatter : public TableFormatter {
public:
  BridgeTableFormatter() = default;

  /**
   * @brief Format bridge interfaces into a detailed table
   * @param interfaces List of ConfigData with bridge configurations
   * @return Formatted ASCII table string
   */
  std::string format(const std::vector<ConfigData> &interfaces) const;
};
