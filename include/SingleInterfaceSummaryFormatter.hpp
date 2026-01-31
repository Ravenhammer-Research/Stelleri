/**
 * @file SingleInterfaceSummaryFormatter.hpp
 * @brief Formatter for detailed single interface view
 */

#pragma once

#include "ConfigData.hpp"
#include <string>

/**
 * @brief Formats a single interface as detailed summary
 *
 * Shows all interface properties in a human-readable format.
 */
class SingleInterfaceSummaryFormatter {
public:
  SingleInterfaceSummaryFormatter() = default;

  /**
   * @brief Format a single interface into a detailed summary
   * @param data ConfigData containing interface information
   * @return Formatted summary string
   */
  std::string format(const ConfigData &data) const;
};
