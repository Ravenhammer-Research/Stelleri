#pragma once

#include "ConfigurationManager.hpp"
#include <memory>

class CLI {
public:
  explicit CLI(std::unique_ptr<ConfigurationManager> mgr);
  ~CLI();
  void run();
  void processLine(const std::string &line);

private:
  std::unique_ptr<ConfigurationManager> mgr_;
};
