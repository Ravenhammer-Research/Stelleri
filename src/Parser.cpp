#include "Parser.hpp"
#include "DeleteCommand.hpp"
#include "IPAddress.hpp"
#include "IPNetwork.hpp"
#include "InterfaceToken.hpp"
#include "RouteToken.hpp"
#include "SetCommand.hpp"
#include "ShowCommand.hpp"
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
      if (sub == "interfaces") {
        size_t next = 0;
        auto iftok = InterfaceToken::parseFromTokens(tokens, i, next);
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
        cmd->addToken(std::make_shared<VRFToken>(tokens[i + 1]));
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
      if (sub == "interfaces") {
        size_t next = 0;
        auto iftok = InterfaceToken::parseFromTokens(tokens, i, next);
        cmd->addToken(iftok);
        return cmd;
      }

      if (sub == "route") {
        if (i + 1 < tokens.size()) {
          auto routeTok = std::make_shared<RouteToken>(tokens[i + 1]);
          size_t j = i + 2;
          j = routeTok->parseOptions(tokens, j);
          cmd->addToken(routeTok);
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
      if (sub == "interfaces") {
        size_t next = 0;
        auto iftok = InterfaceToken::parseFromTokens(tokens, i, next);
        cmd->addToken(iftok);
        return cmd;
      }
      // unknown delete target: return `delete` only
      return cmd;
    }

    // Unrecognized top-level verb: return empty command (no tokens)
    return cmd;
  }
} // namespace netcli
