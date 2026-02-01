#pragma once

/**
 * @file ConfigData.hpp
 * @brief Lightweight marker/container for configuration payloads
 */

#include <ifaddrs.h>
#include <string_view>
#include <cstring>

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

	// Persist this configuration to the system. Implementations must
	// provide a concrete `save` method.
	virtual void save() const = 0;

	// Destroy this configuration on the system (e.g., remove interface).
	// Concrete types should implement this to perform deletion/teardown.
	virtual void destroy() const = 0;

	// Check whether the named interface exists on the system. This is
	// provided as a convenience for subclasses and callers.
	static bool exists(std::string_view name) {
		bool found = false;
		struct ifaddrs *ifs = nullptr;
		if (getifaddrs(&ifs) == 0) {
			for (struct ifaddrs *ifa = ifs; ifa; ifa = ifa->ifa_next) {
				if (ifa->ifa_name && name == ifa->ifa_name) {
					found = true;
					break;
				}
			}
			freeifaddrs(ifs);
		}
		return found;
	}

protected:
	// Protected constructor prevents direct instantiation; subclassing
	// is required to provide concrete payloads and the `save` implementation.
	ConfigData() = default;
};
