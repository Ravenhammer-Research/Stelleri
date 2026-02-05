#pragma once

#include "InterfaceConfig.hpp"

class CarpConfig : public InterfaceConfig {
public:
  explicit CarpConfig(const InterfaceConfig &base) : InterfaceConfig(base) {}
  void save() const override;
  void create() const;
};
