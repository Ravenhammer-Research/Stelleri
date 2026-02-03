/**
 * @file LaggTableFormatter.hpp
 * @brief Formatter for LAGG interface details
 */

#pragma once

#include "ConfigData.hpp"
#include "TableFormatter.hpp"
#include <string>
#include <vector>

/**
 * @brief Formats LAGG interface configuration as ASCII table
 *
 * Shows LAGG-specific details like protocol, members, hash policy, LACP
 * settings.
 */
class LaggTableFormatter : public TableFormatter {
public:
  LaggTableFormatter() = default;

  /**
   * @brief Format LAGG interfaces into a detailed table
   * @param interfaces List of ConfigData with LAGG configurations
   * @return Formatted ASCII table string
   */
  std::string format(const std::vector<ConfigData> &interfaces) const;
};
