#include "Parser.hpp"
#include "DeleteCommand.hpp"
#include "IPAddress.hpp"
#include "IPNetwork.hpp"
#include "InterfaceToken.hpp"
#include "ProtocolsToken.hpp"
#include "RouteToken.hpp"
#include "SetCommand.hpp"
#include "ShowCommand.hpp"
#include "StaticToken.hpp"
#include "Token.hpp"
#include "VRFToken.hpp"
#include <iostream>

namespace netcli {
  std::unique_ptr<Command> parse(const std::vector<std::string> &tokens) {
    auto cmd = std::make_unique<Command>();
    if (tokens.empty())
      return cmd;

    // Debug: show incoming tokens
    std::cerr << "[parser] tokens:";
    for (const auto &t : tokens)
      std::cerr << ' ' << t;
    std::cerr << '\n';

    size_t i = 0;
    const std::string verb = tokens[i];
    std::cerr << "[parser] verb='" << verb << "'\n";

    if (verb == "show") {
      cmd->addToken(std::make_shared<ShowCommand>());
      ++i;
      if (i >= tokens.size())
        return cmd;
      const std::string sub = tokens[i];
      if (sub == "interfaces" || sub == "interface") {
        std::shared_ptr<InterfaceToken> iftok;
        
        if (i + 1 < tokens.size() && tokens[i + 1] == "name" && i + 2 < tokens.size()) {
          iftok = std::make_shared<InterfaceToken>(InterfaceType::Unknown, tokens[i + 2]);
        } else if (i + 1 < tokens.size() && tokens[i + 1] == "type" && i + 2 < tokens.size()) {
          std::string typeStr = tokens[i + 2];
          InterfaceType itype = InterfaceType::Unknown;
          if (typeStr == "ethernet") itype = InterfaceType::Ethernet;
          else if (typeStr == "loopback") itype = InterfaceType::Loopback;
          else if (typeStr == "bridge") itype = InterfaceType::Bridge;
          else if (typeStr == "vlan") itype = InterfaceType::VLAN;
          else if (typeStr == "wireless") itype = InterfaceType::Wireless;
          else if (typeStr == "ppp") itype = InterfaceType::PPP;
          else if (typeStr == "tunnel") itype = InterfaceType::Tunnel;
          else if (typeStr == "point-to-point") itype = InterfaceType::PointToPoint;
          else if (typeStr == "virtual" || typeStr == "epair") itype = InterfaceType::Virtual;
          iftok = std::make_shared<InterfaceToken>(itype, "");
        } else {
          size_t next = 0;
          iftok = InterfaceToken::parseFromTokens(tokens, i, next);
        }
        
        cmd->addToken(iftok);
        return cmd;
      }

      if (sub == "route" || sub == "routes") {
        // "show routes" -> show all routes (empty prefix)
        // "show route <prefix>" -> show specific route
        std::string prefix =
            (sub == "routes" || i + 1 >= tokens.size()) ? "" : tokens[i + 1];
        auto routeTok = std::make_shared<RouteToken>(prefix);
        size_t j = (sub == "routes" || prefix.empty()) ? i + 1 : i + 2;
        j = routeTok->parseOptions(tokens, j);
        cmd->addToken(routeTok);
        return cmd;
      }

      if (sub == "vrf" && i + 1 < tokens.size()) {
        auto vrfTok = std::make_shared<VRFToken>(tokens[i + 1]);
        size_t j = i + 2;
        // Parse VRF attributes
        while (j < tokens.size()) {
          if (tokens[j] == "table" && j + 1 < tokens.size()) {
            try {
              vrfTok->table = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid table number: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else {
            break;
          }
        }
        cmd->addToken(vrfTok);
        return cmd;
      }

      // unknown subcommand: return command with only `show`
      return cmd;
    }

    if (verb == "set") {
      cmd->addToken(std::make_shared<SetCommand>());
      ++i;
      if (i >= tokens.size())
        return cmd;
      const std::string sub = tokens[i];
      if (sub == "interfaces" || sub == "interface") {
        size_t next = 0;
        auto iftok = InterfaceToken::parseFromTokens(tokens, i, next);
        
        // Parse interface-specific attributes
        size_t j = next;
        while (j < tokens.size()) {
          const std::string &attr = tokens[j];
          
          if (attr == "address" && j + 1 < tokens.size()) {
            iftok->address = tokens[j + 1];
            j += 2;
          } else if (attr == "mtu" && j + 1 < tokens.size()) {
            try {
              iftok->mtu = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid MTU: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "vrf" && j + 1 < tokens.size()) {
            iftok->vrf = tokens[j + 1];
            j += 2;
          } else if (attr == "tunnel-vrf" && j + 1 < tokens.size()) {
            try {
              iftok->tunnel_vrf = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid tunnel-vrf FIB ID: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "tunnel-source" && j + 1 < tokens.size()) {
            if (!iftok->tunnel) {
              iftok->tunnel = TunnelConfig();
            }
            iftok->tunnel->source = tokens[j + 1];
            j += 2;
          } else if (attr == "tunnel-destination" && j + 1 < tokens.size()) {
            if (!iftok->tunnel) {
              iftok->tunnel = TunnelConfig();
            }
            iftok->tunnel->destination = tokens[j + 1];
            j += 2;
          } else if (attr == "member" && j + 1 < tokens.size()) {
            // Bridge or LAGG member
            if (!iftok->bridge && !iftok->lagg) {
              // Initialize based on interface type
              if (iftok->type() == InterfaceType::Bridge) {
                iftok->bridge = BridgeInterfaceConfig();
              } else {
                iftok->lagg = LaggConfig();
              }
            }
            if (iftok->bridge) {
              iftok->bridge->members.push_back(tokens[j + 1]);
            } else if (iftok->lagg) {
              iftok->lagg->members.push_back(tokens[j + 1]);
            }
            j += 2;
          } else if (attr == "protocol" && j + 1 < tokens.size()) {
            // LAGG protocol
            if (!iftok->lagg) {
              iftok->lagg = LaggConfig();
            }
            const std::string &proto = tokens[j + 1];
            if (proto == "lacp") {
              iftok->lagg->protocol = LaggProtocol::LACP;
            } else if (proto == "failover") {
              iftok->lagg->protocol = LaggProtocol::FAILOVER;
            } else if (proto == "loadbalance" || proto == "loadbalancing") {
              iftok->lagg->protocol = LaggProtocol::LOADBALANCE;
            }
            j += 2;
          } else if (attr == "stp" && j + 1 < tokens.size()) {
            // Bridge STP
            if (!iftok->bridge) {
              iftok->bridge = BridgeInterfaceConfig();
            }
            const std::string &val = tokens[j + 1];
            iftok->bridge->stp = (val == "true" || val == "1" || val == "enable");
            j += 2;
          } else if (attr == "priority" && j + 1 < tokens.size()) {
            // Bridge priority
            if (!iftok->bridge) {
              iftok->bridge = BridgeInterfaceConfig();
            }
            try {
              iftok->bridge->priority = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid bridge priority: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "hello-time" && j + 1 < tokens.size()) {
            // Bridge hello time
            if (!iftok->bridge) {
              iftok->bridge = BridgeInterfaceConfig();
            }
            try {
              iftok->bridge->hello_time = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid hello-time: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "forward-delay" && j + 1 < tokens.size()) {
            // Bridge forward delay
            if (!iftok->bridge) {
              iftok->bridge = BridgeInterfaceConfig();
            }
            try {
              iftok->bridge->forward_delay = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid forward-delay: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "max-age" && j + 1 < tokens.size()) {
            // Bridge max age
            if (!iftok->bridge) {
              iftok->bridge = BridgeInterfaceConfig();
            }
            try {
              iftok->bridge->max_age = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid max-age: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "aging-time" && j + 1 < tokens.size()) {
            // Bridge aging time
            if (!iftok->bridge) {
              iftok->bridge = BridgeInterfaceConfig();
            }
            try {
              iftok->bridge->aging_time = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid aging-time: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "max-addresses" && j + 1 < tokens.size()) {
            // Bridge max addresses
            if (!iftok->bridge) {
              iftok->bridge = BridgeInterfaceConfig();
            }
            try {
              iftok->bridge->max_addresses = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid max-addresses: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "hash-policy" && j + 1 < tokens.size()) {
            // LAGG hash policy
            if (!iftok->lagg) {
              iftok->lagg = LaggConfig();
            }
            iftok->lagg->hash_policy = tokens[j + 1];
            j += 2;
          } else if (attr == "lacp-rate" && j + 1 < tokens.size()) {
            // LACP rate (0=slow/30s, 1=fast/1s)
            if (!iftok->lagg) {
              iftok->lagg = LaggConfig();
            }
            try {
              iftok->lagg->lacp_rate = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid lacp-rate: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "min-links" && j + 1 < tokens.size()) {
            // Minimum active links
            if (!iftok->lagg) {
              iftok->lagg = LaggConfig();
            }
            try {
              iftok->lagg->min_links = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid min-links: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "pcp" && j + 1 < tokens.size()) {
            // VLAN Priority Code Point
            if (!iftok->vlan) {
              iftok->vlan = VLANConfig();
            }
            try {
              iftok->vlan->pcp = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid PCP: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "vlan-id" && j + 1 < tokens.size()) {
            // VLAN ID
            if (!iftok->vlan) {
              iftok->vlan = VLANConfig();
            }
            try {
              iftok->vlan->id = std::stoi(tokens[j + 1]);
            } catch (...) {
              std::cerr << "[parser] Invalid VLAN ID: " << tokens[j + 1] << "\n";
            }
            j += 2;
          } else if (attr == "parent" && j + 1 < tokens.size()) {
            // VLAN parent interface
            if (!iftok->vlan) {
              iftok->vlan = VLANConfig();
            }
            iftok->vlan->parent = tokens[j + 1];
            j += 2;
          } else {
            break;
          }
        }
        
        cmd->addToken(iftok);
        return cmd;
      }

      if (sub == "route") {
        // Expect: set route protocols static <prefix> ...
        if (i + 1 < tokens.size() && tokens[i + 1] == "protocols") {
          if (i + 2 < tokens.size() && tokens[i + 2] == "static") {
            if (i + 3 < tokens.size()) {
              // Create token chain: RouteToken -> ProtocolsToken -> StaticToken
              auto routeTok = std::make_shared<RouteToken>(tokens[i + 3]);
              auto protoTok = std::make_shared<ProtocolsToken>("static");
              auto staticTok = std::make_shared<StaticToken>();
              
              cmd->addToken(routeTok);
              routeTok->setNext(protoTok);
              protoTok->setNext(staticTok);
              
              // Parse route options (next-hop, interface, vrf, blackhole, reject)
              size_t j = i + 4;
              j = routeTok->parseOptions(tokens, j);
              
              return cmd;
            } else {
              std::cerr << "[parser] Error: Missing route prefix after 'protocols static'\n";
              return cmd;
            }
          } else {
            std::cerr << "[parser] Error: Expected 'static' after 'protocols'\n";
            return cmd;
          }
        } else {
          std::cerr << "[parser] Error: Expected 'protocols' after 'route'\n";
          return cmd;
        }
      }

      // unknown set target: return `set` only
      return cmd;
    }

    if (verb == "delete") {
      cmd->addToken(std::make_shared<DeleteCommand>());
      ++i;
      if (i >= tokens.size())
        return cmd;
      const std::string sub = tokens[i];
      if (sub == "interfaces" || sub == "interface") {
        size_t next = 0;
        auto iftok = InterfaceToken::parseFromTokens(tokens, i, next);
        cmd->addToken(iftok);
        return cmd;
      }
      if (sub == "route") {
        // Expect: delete route protocols static <prefix>
        if (i + 1 < tokens.size() && tokens[i + 1] == "protocols") {
          if (i + 2 < tokens.size() && tokens[i + 2] == "static") {
            if (i + 3 < tokens.size()) {
              auto routeTok = std::make_shared<RouteToken>(tokens[i + 3]);
              auto protoTok = std::make_shared<ProtocolsToken>("static");
              auto staticTok = std::make_shared<StaticToken>();
              
              cmd->addToken(routeTok);
              routeTok->setNext(protoTok);
              protoTok->setNext(staticTok);
              
              // Parse route options for VRF context if present
              size_t j = i + 4;
              j = routeTok->parseOptions(tokens, j);
              
              return cmd;
            } else {
              std::cerr << "[parser] Error: Missing route prefix after 'protocols static'\n";
              return cmd;
            }
          } else {
            std::cerr << "[parser] Error: Expected 'static' after 'protocols'\n";
            return cmd;
          }
        } else {
          std::cerr << "[parser] Error: Expected 'protocols' after 'route'\n";
          return cmd;
        }
      }
      // unknown delete target: return `delete` only
      return cmd;
    }

    // Unrecognized top-level verb: return empty command (no tokens)
    return cmd;
  }
} // namespace netcli
