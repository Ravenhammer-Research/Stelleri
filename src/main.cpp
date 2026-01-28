#include "CLI.hpp"
#include "SystemConfigurationManager.hpp"
#include <getopt.h>
#include <iostream>

int main(int argc, char *argv[]) {
  bool interactive = true;
  std::string onecmd;

  struct option longopts[] = {{"command", required_argument, nullptr, 'c'},
                              {"file", required_argument, nullptr, 'f'},
                              {"interactive", no_argument, nullptr, 'i'},
                              {"help", no_argument, nullptr, 'h'},
                              {0, 0, 0, 0}};

  int ch;
  while ((ch = getopt_long(argc, argv, "c:f:ih", longopts, nullptr)) != -1) {
    switch (ch) {
    case 'c':
      onecmd = optarg;
      interactive = false;
      break;
    case 'i':
      interactive = true;
      break;
    case 'h':
      std::cout << "Usage: netcli [-c command] [-i]\n";
      return 0;
    default:
      break;
    }
  }

  auto mgr = std::make_unique<SystemConfigurationManager>();
  CLI cli(std::move(mgr));

  if (!onecmd.empty()) {
    // simple one-shot
    cli.processLine(onecmd);
    return 0;
  }

  cli.run();
  return 0;
}
