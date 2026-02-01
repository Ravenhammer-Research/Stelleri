/**
 * @file PriorityCodePoint.hpp
 * @brief VLAN Priority Code Point (PCP) enumeration
 */

#pragma once

/**
 * @brief 802.1Q Priority Code Point (PCP) values (3-bit)
 */
enum class PriorityCodePoint : int {
  PCP0 = 0, // Best Effort
  PCP1 = 1, // Background
  PCP2 = 2, // Excellent Effort / Spare
  PCP3 = 3, // Critical Applications
  PCP4 = 4, // Video
  PCP5 = 5, // Voice
  PCP6 = 6, // Internetwork Control
  PCP7 = 7  // Network Control
};
