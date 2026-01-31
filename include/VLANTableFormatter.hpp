/**
 * @file VLANTableFormatter.hpp
 * @brief Formatter for VLAN interface details
 */

#pragma once

#include "ConfigData.hpp"
#include <string>
#include <vector>

/**
 * @brief Formats VLAN interface configuration as ASCII table
 * 
 * Shows VLAN-specific details like VLAN ID, parent interface, PCP.
 */
class VLANTableFormatter {
public:
  VLANTableFormatter() = default;

  /**
   * @brief Format VLAN interfaces into a detailed table
   * @param interfaces List of ConfigData with VLAN configurations
   * @return Formatted ASCII table string
   */
  std::string format(const std::vector<ConfigData> &interfaces) const;
};
