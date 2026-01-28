#pragma once

#include "ConfigData.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class ConfigurationManager {
public:
  enum class Type { Memory };
  virtual ~ConfigurationManager() = default;

  // Abstract operations used by the CLI/parser
  virtual void set(const std::string &path, const ConfigData &data) = 0;
  virtual void delete_config(const std::string &path) = 0;

  // Query configuration data
  virtual std::optional<ConfigData>
  getInterface(const std::string &name) const = 0;

  virtual std::vector<ConfigData> getInterfaces() const = 0;

  virtual std::vector<ConfigData> getRoutes() const = 0;

  // Factory
  static std::unique_ptr<ConfigurationManager> create(Type t);

protected:
  // Implementation-specific helpers may be provided by subclasses.
};
