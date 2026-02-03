#include "CLI.hpp"
#include "DeleteCommand.hpp"
#include "InterfaceToken.hpp"
#include "Parser.hpp"
#include "RouteToken.hpp"
#include "VRFToken.hpp"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

CLI::CLI(std::unique_ptr<ConfigurationManager> mgr) : mgr_(std::move(mgr)) {
  // Set history file to ~/.netcli_history
  const char *home = getenv("HOME");
  if (home) {
    historyFile_ = std::string(home) + "/.netcli_history";
  }
  loadHistory();
}

CLI::~CLI() {}

void CLI::loadHistory() {
  // History is loaded on demand during interactive mode
}

void CLI::saveHistory(const std::string &line) {
  if (historyFile_.empty() || line.empty())
    return;
  std::ofstream hist(historyFile_, std::ios::app);
  if (hist.is_open()) {
    hist << line << '\n';
  }
}

void CLI::processLine(const std::string &line) {
  if (line.empty())
    return;
  if (line == "exit" || line == "quit") {
    std::exit(0);
  }

  // Save to history but do not perform any system changes here — stubbed.
  saveHistory(line);

  netcli::Parser parser;
  auto toks = parser.tokenize(line);
  auto cmd = parser.parse(toks);
  if (!cmd || !cmd->head()) {
    std::cerr << "Error: Invalid command\n";
    return;
  }

  // Dispatch parsed command to executor via Parser instance
  parser.executeCommand(cmd->head(), mgr_.get());
}

void CLI::run() {
  std::string line;
  while (true) {
    std::cout << "net> " << std::flush;
    if (!std::getline(std::cin, line))
      break; // EOF
    if (line.empty())
      continue;
    processLine(line);
  }
}
