#pragma once

/**
 * @file ConfigData.hpp
 * @brief Lightweight marker/container for configuration payloads
 */

#include <cstring>
#include <ifaddrs.h>
#include <memory>
#include <string_view>

class InterfaceConfig; // forward
class RouteConfig;     // forward

/**
 * @brief Lightweight configuration container/base type
 *
 * Intentionally keeps no owning members to avoid circular-include
 * and ownership responsibilities. Concrete configuration types
 * (e.g., `InterfaceConfig`) may inherit from this type.
 */
class ConfigData {
public:
  virtual ~ConfigData() = default;

  // Persist this configuration to the system. Implementations may
  // override to provide concrete behavior. Default is no-op.
  virtual void save() const {}

  // Destroy this configuration on the system (e.g., remove interface).
  // Concrete types may override to perform deletion/teardown. Default is no-op.
  virtual void destroy() const {}

  // (Interface existence helper moved to `InterfaceConfig`)

  // Optional pointer to an InterfaceConfig payload used by formatters and
  // consumers that aggregate heterogeneous configuration objects.
  // Use shared_ptr so it can be declared with a forward declaration.
  std::shared_ptr<InterfaceConfig> iface;
  // Optional pointer to a RouteConfig when this ConfigData carries a route
  std::shared_ptr<RouteConfig> route;

public:
  // Default constructor allows creating lightweight container instances
  // that hold `iface` or `route` payload pointers.
  ConfigData() = default;
};
