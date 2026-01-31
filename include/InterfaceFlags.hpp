/**
 * @file InterfaceFlags.hpp
 * @brief Interface flag definitions
 */

#pragma once

#include <cstdint>

/**
 * @brief FreeBSD interface flags
 * 
 * Matches FreeBSD's net/if.h IFF_* constants.
 */
enum class InterfaceFlag : uint32_t {
  UP          = 0x1,      ///< Interface is up
  BROADCAST   = 0x2,      ///< Broadcast address valid
  DEBUG       = 0x4,      ///< Turn on debugging
  LOOPBACK    = 0x8,      ///< Is a loopback interface
  POINTOPOINT = 0x10,     ///< Point-to-point link
  RUNNING     = 0x40,     ///< Resources allocated
  NOARP       = 0x80,     ///< No address resolution protocol
  PROMISC     = 0x100,    ///< Receive all packets
  ALLMULTI    = 0x200,    ///< Receive all multicast packets
  MULTICAST   = 0x8000,   ///< Supports multicast
};

/**
 * @brief Convert interface flags bitmask to human-readable string
 * @param flags Bitmask of interface flags
 * @return String representation (e.g., "UBRM")
 */
inline std::string flagsToString(uint32_t flags) {
  std::string result;
  if (flags & static_cast<uint32_t>(InterfaceFlag::UP)) result += "U";
  if (flags & static_cast<uint32_t>(InterfaceFlag::BROADCAST)) result += "B";
  if (flags & static_cast<uint32_t>(InterfaceFlag::DEBUG)) result += "D";
  if (flags & static_cast<uint32_t>(InterfaceFlag::LOOPBACK)) result += "L";
  if (flags & static_cast<uint32_t>(InterfaceFlag::POINTOPOINT)) result += "P";
  if (flags & static_cast<uint32_t>(InterfaceFlag::RUNNING)) result += "R";
  if (flags & static_cast<uint32_t>(InterfaceFlag::NOARP)) result += "N";
  if (flags & static_cast<uint32_t>(InterfaceFlag::PROMISC)) result += "O";
  if (flags & static_cast<uint32_t>(InterfaceFlag::ALLMULTI)) result += "A";
  if (flags & static_cast<uint32_t>(InterfaceFlag::MULTICAST)) result += "M";
  return result.empty() ? "-" : result;
}
