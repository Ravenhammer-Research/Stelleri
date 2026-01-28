#include "CLI.hpp"
#include "InterfaceToken.hpp"
#include "Parser.hpp"
#include "RouteToken.hpp"
#include "ShowCommand.hpp"
#include "Tokenizer.hpp"
#include <iostream>

CLI::CLI(std::unique_ptr<ConfigurationManager> mgr) : mgr_(std::move(mgr)) {}
CLI::~CLI() {}

void CLI::processLine(const std::string &line) {
  if (line.empty())
    return;
  if (line == "exit" || line == "quit") {
    std::exit(0);
  }
  auto toks = netcli::tokenize(line);
  auto cmd = netcli::parse(toks);
  if (cmd) {
    auto head = cmd->head();
    if (head && dynamic_cast<ShowCommand *>(head.get())) {
      auto next = head->getNext();
      if (next) {
        if (auto iftok = dynamic_cast<InterfaceToken *>(next.get())) {
          std::cout << iftok->renderTable(mgr_.get());
          return;
        }
        if (auto routeTok = dynamic_cast<RouteToken *>(next.get())) {
          std::cout << routeTok->renderTable(mgr_.get());
          return;
        }
      }
    }
    std::cout << "Parsed: " << cmd->toString() << '\n';
  }
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
