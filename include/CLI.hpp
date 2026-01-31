/**
 * @file CLI.hpp
 * @brief Command-line interface implementation
 */

#pragma once

#include "ConfigurationManager.hpp"
#include <memory>
#include <string>

/**
 * @brief VyOS-style command-line interface
 *
 * Provides an interactive REPL for network configuration with
 * command history persistence.
 */
class CLI {
public:
  /**
   * @brief Construct CLI with a configuration manager
   * @param mgr Configuration manager implementation
   */
  explicit CLI(std::unique_ptr<ConfigurationManager> mgr);
  ~CLI();

  /**
   * @brief Start interactive REPL loop
   */
  void run();

  /**
   * @brief Process a single command line
   * @param line Command string to execute
   */
  void processLine(const std::string &line);

private:
  std::unique_ptr<ConfigurationManager> mgr_;
  std::string historyFile_;
  void loadHistory();
  void saveHistory(const std::string &line);
};
