#pragma once

#include "ConfigurationManager.hpp"
#include <vector>

class SystemConfigurationManager : public ConfigurationManager {
public:
  SystemConfigurationManager() = default;
  ~SystemConfigurationManager() override = default;

  void set(const std::string &path, const ConfigData &data) override;
  void delete_config(const std::string &path) override;

protected:
  std::optional<ConfigData>
  getInterface(const std::string &name) const override;
  std::vector<ConfigData> getInterfaces() const override;
  std::vector<ConfigData> getRoutes() const override;
};
