#pragma once

#include "InterfaceConfig.hpp"

class WlanConfig : public InterfaceConfig {
public:
  explicit WlanConfig(const InterfaceConfig &base) : InterfaceConfig(base) {}
  void save() const override;
  void create() const;
};
