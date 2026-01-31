/**
 * @file ConfigurationManager.hpp
 * @brief Abstract configuration management interface
 */

#pragma once

#include "ConfigData.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Abstract base class for configuration storage and retrieval
 * 
 * Provides an interface for setting and querying network configuration data.
 * Implementations can use in-memory storage, file-based persistence, or other backends.
 */
class ConfigurationManager {
public:
  /** @brief Configuration storage type */
  enum class Type { Memory };
  
  virtual ~ConfigurationManager() = default;

  /**
   * @brief Set or update configuration at specified path
   * @param path Configuration path (e.g., "interfaces.eth0")
   * @param data Configuration data to store
   */
  virtual void set(const std::string &path, const ConfigData &data) = 0;
  
  /**
   * @brief Delete configuration at specified path
   * @param path Configuration path to delete
   */
  virtual void delete_config(const std::string &path) = 0;

  /**
   * @brief Get configuration for a specific interface
   * @param name Interface name
   * @return ConfigData if found, std::nullopt otherwise
   */
  virtual std::optional<ConfigData>
  getInterface(const std::string &name) const = 0;

  /**
   * @brief Get all interface configurations
   * @return Vector of interface ConfigData objects
   */
  virtual std::vector<ConfigData> getInterfaces() const = 0;

  /**
   * @brief Get all route configurations
   * @return Vector of route ConfigData objects
   */
  virtual std::vector<ConfigData> getRoutes() const = 0;

  /**
   * @brief Factory method to create a configuration manager
   * @param t Type of configuration manager to create
   * @return Unique pointer to ConfigurationManager instance
   */
  static std::unique_ptr<ConfigurationManager> create(Type t);

protected:
  // Implementation-specific helpers may be provided by subclasses.
};
