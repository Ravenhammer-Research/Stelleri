/**
 * @file InterfaceTableFormatter.hpp
 * @brief General-purpose interface table formatter
 */

#pragma once

#include "ConfigData.hpp"
#include <string>
#include <vector>

/**
 * @brief Formats interface configuration data as ASCII tables
 *
 * General-purpose formatter that handles mixed interface types.
 * For uniform interface types, specialized formatters may be preferred.
 */
class InterfaceTableFormatter {
public:
  InterfaceTableFormatter() = default;

  /**
   * @brief Format interface list as ASCII table
   * @param interfaces Vector of interface ConfigData objects
   * @return Formatted table string with interface information
   */
  std::string format(const std::vector<ConfigData> &interfaces) const;
};
