#pragma once

#include "InterfaceConfig.hpp"

class TapConfig : public InterfaceConfig {
public:
  explicit TapConfig(const InterfaceConfig &base) : InterfaceConfig(base) {}
  void save() const override;
  void create() const;
};
