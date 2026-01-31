/**
 * @file VirtualTableFormatter.hpp
 * @brief Formatter for virtual interface details
 */

#pragma once

#include "ConfigData.hpp"
#include <string>
#include <vector>

/**
 * @brief Formats virtual interface configuration as ASCII table
 * 
 * Shows virtual interface details like epair peers, tap devices, routing domain.
 */
class VirtualTableFormatter {
public:
  VirtualTableFormatter() = default;

  /**
   * @brief Format virtual interfaces into a detailed table
   * @param interfaces List of ConfigData with virtual interface configurations
   * @return Formatted ASCII table string
   */
  std::string format(const std::vector<ConfigData> &interfaces) const;
};
