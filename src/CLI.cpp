#include "CLI.hpp"
#include "DeleteCommand.hpp"
#include "InterfaceToken.hpp"
#include "Parser.hpp"
#include "RouteToken.hpp"
#include "SetCommand.hpp"
#include "ShowCommand.hpp"
#include "Tokenizer.hpp"
#include "VRFToken.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

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
  
  // Save to history
  saveHistory(line);
  
  auto toks = netcli::tokenize(line);
  auto cmd = netcli::parse(toks);
  if (!cmd || !cmd->head()) {
    std::cerr << "Error: Invalid command\n";
    return;
  }

  auto head = cmd->head();

  // Handle show commands
  if (dynamic_cast<ShowCommand *>(head.get())) {
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
      if (auto vrfTok = dynamic_cast<VRFToken *>(next.get())) {
        std::cout << "VRF: " << vrfTok->name() << "\n";
        // TODO: Implement VRF display
        return;
      }
    }
    std::cout << "Parsed: " << cmd->toString() << "\n";
    return;
  }

  // Handle set commands
  if (dynamic_cast<SetCommand *>(head.get())) {
    auto next = head->getNext();
    if (!next) {
      std::cerr << "Error: Incomplete set command\n";
      return;
    }

    if (auto iftok = dynamic_cast<InterfaceToken *>(next.get())) {
      try {
        // Build ConfigData from InterfaceToken
        ConfigData cd;
        InterfaceConfig ic;
        ic.name = iftok->name();
        ic.type = iftok->type();
        
        if (iftok->address) {
          ic.address = IPNetwork::fromString(*iftok->address);
        }
        if (iftok->mtu) {
          ic.mtu = *iftok->mtu;
        }
        if (iftok->vrf) {
          VRFConfig vrf;
          vrf.name = *iftok->vrf;
          ic.vrf = vrf;
        }
        if (iftok->tunnel_vrf) {
          ic.tunnel_vrf = *iftok->tunnel_vrf;
        }
        if (iftok->bridge) {
          ic.bridge = *iftok->bridge;
        }
        if (iftok->lagg) {
          ic.lagg = *iftok->lagg;
        }
        if (iftok->vlan) {
          ic.vlan = *iftok->vlan;
        }
        if (iftok->tunnel) {
          ic.tunnel = *iftok->tunnel;
        }
        
        cd.iface = std::move(ic);
        
        // Call configuration manager
        std::string path = "interfaces." + iftok->name();
        mgr_->set(path, cd);
        std::cout << "Interface " << iftok->name() << " configured successfully\n";
      } catch (const std::exception &e) {
        std::cerr << "Error configuring interface: " << e.what() << "\n";
      }
      return;
    }

    if (auto routeTok = dynamic_cast<RouteToken *>(next.get())) {
      try {
        ConfigData cd;
        RouteConfig rc;
        rc.prefix = routeTok->prefix();
        
        if (routeTok->nexthop) {
          rc.nexthop = routeTok->nexthop->toString();
        }
        if (routeTok->interface) {
          rc.iface = routeTok->interface->name();
        }
        if (routeTok->vrf) {
          rc.vrf = routeTok->vrf->name();
        }
        rc.blackhole = routeTok->blackhole;
        rc.reject = routeTok->reject;
        
        cd.route = std::move(rc);
        
        std::string path = "route." + routeTok->prefix();
        mgr_->set(path, cd);
        std::cout << "Route " << routeTok->prefix() << " configured successfully\n";
      } catch (const std::exception &e) {
        std::cerr << "Error configuring route: " << e.what() << "\n";
      }
      return;
    }

    if (auto vrfTok = dynamic_cast<VRFToken *>(next.get())) {
      try {
        ConfigData cd;
        VRFConfig vc;
        vc.name = vrfTok->name();        if (vrfTok->table) {
          vc.table = vrfTok->table;
        }
        cd.vrf = std::move(vc);
        
        std::string path = "vrf." + vrfTok->name();
        mgr_->set(path, cd);
        std::cout << "VRF " << vrfTok->name() << " configured successfully\n";
      } catch (const std::exception &e) {
        std::cerr << "Error configuring VRF: " << e.what() << "\n";
      }
      return;
    }

    std::cerr << "Error: Unknown set target\n";
    return;
  }

  // Handle delete commands
  if (dynamic_cast<DeleteCommand *>(head.get())) {
    auto next = head->getNext();
    if (!next) {
      std::cerr << "Error: Incomplete delete command\n";
      return;
    }

    if (auto iftok = dynamic_cast<InterfaceToken *>(next.get())) {
      try {
        std::string path = "interfaces." + iftok->name();
        mgr_->delete_config(path);
        std::cout << "Interface " << iftok->name() << " deleted successfully\n";
      } catch (const std::exception &e) {
        std::cerr << "Error deleting interface: " << e.what() << "\n";
      }
      return;
    }

    if (auto routeTok = dynamic_cast<RouteToken *>(next.get())) {
      try {
        std::string path = "route." + routeTok->prefix();
        mgr_->delete_config(path);
        std::cout << "Route " << routeTok->prefix() << " deleted successfully\n";
      } catch (const std::exception &e) {
        std::cerr << "Error deleting route: " << e.what() << "\n";
      }
      return;
    }

    if (auto vrfTok = dynamic_cast<VRFToken *>(next.get())) {
      try {
        std::string path = "vrf." + vrfTok->name();
        mgr_->delete_config(path);
        std::cout << "VRF " << vrfTok->name() << " deleted successfully\n";
      } catch (const std::exception &e) {
        std::cerr << "Error deleting VRF: " << e.what() << "\n";
      }
      return;
    }

    std::cerr << "Error: Unknown delete target\n";
    return;
  }

  std::cout << "Parsed: " << cmd->toString() << "\n";
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
