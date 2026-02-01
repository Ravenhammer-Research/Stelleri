#pragma once

#include "Token.hpp"
#include "ConfigurationManager.hpp"
#include "InterfaceToken.hpp"
#include "RouteToken.hpp"
#include "VRFToken.hpp"
#include <memory>
#include <string>
#include <vector>

namespace netcli {


class Parser {
public:
  Parser() = default;
  ~Parser() = default;

  // Tokenize a raw command line into space-separated tokens.
  std::vector<std::string> tokenize(const std::string &line) const;

  // Parse a token vector into a Command. Returns nullptr on parse error.
  std::unique_ptr<Command> parse(const std::vector<std::string> &tokens) const;

  // Main dispatcher: execute a parsed command token chain.
  void executeCommand(const std::shared_ptr<Token> &head, ConfigurationManager *mgr) const;

  // Per-target execute handlers (moved from standalone Execute* files).
  void executeShowInterface(const InterfaceToken &tok, ConfigurationManager *mgr) const;
  void executeSetInterface(const InterfaceToken &tok, ConfigurationManager *mgr) const;
  void executeDeleteInterface(const InterfaceToken &tok, ConfigurationManager *mgr) const;

  void executeShowRoute(const RouteToken &tok, ConfigurationManager *mgr) const;
  void executeSetRoute(const RouteToken &tok, ConfigurationManager *mgr) const;
  void executeDeleteRoute(const RouteToken &tok, ConfigurationManager *mgr) const;

  void executeShowVRF(const VRFToken &tok, ConfigurationManager *mgr) const;
  void executeSetVRF(const VRFToken &tok, ConfigurationManager *mgr) const;
  void executeDeleteVRF(const VRFToken &tok, ConfigurationManager *mgr) const;
};

} // namespace netcli

