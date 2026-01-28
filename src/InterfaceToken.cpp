#include "InterfaceToken.hpp"
#include "InterfaceTableFormatter.hpp"
#include <iostream>

InterfaceToken::InterfaceToken(InterfaceType t, std::string name)
    : type_(t), name_(std::move(name)) {}

std::string InterfaceToken::toString() const {
  std::string t;
  switch (type_) {
  case InterfaceType::Ethernet:
    t = "interfaces ethernet";
    break;
  case InterfaceType::Loopback:
    t = "interfaces loopback";
    break;
  case InterfaceType::PPP:
    t = "interfaces ppp";
    break;
  default:
    t = "interfaces";
    break;
  }
  return t + " " + name_;
}

std::vector<std::string> InterfaceToken::autoComplete(std::string_view) const {
  return {}; // suggestions could query ConfigurationManager; keep empty for
             // now
}

std::unique_ptr<Token> InterfaceToken::clone() const {
  return std::make_unique<InterfaceToken>(*this);
}

std::string InterfaceToken::renderTable(ConfigurationManager *mgr) const {
  if (!mgr)
    return "Error: No configuration manager\n";

  std::vector<ConfigData> interfaces;
  if (name_.empty()) {
    // Get all interfaces
    interfaces = mgr->getInterfaces();
  } else {
    // Get specific interface
    auto cdopt = mgr->getInterface(name_);
    if (cdopt) {
      interfaces.push_back(std::move(*cdopt));
    }
  }

  InterfaceTableFormatter formatter;
  return formatter.format(interfaces);
}

std::shared_ptr<InterfaceToken>
InterfaceToken::parseFromTokens(const std::vector<std::string> &tokens,
                                size_t start, size_t &next) {
  next = start + 1; // by default consume the 'interfaces' token
  if (start + 2 < tokens.size()) {
    const std::string &type = tokens[start + 1];
    const std::string &name = tokens[start + 2];
    InterfaceType itype = InterfaceType::Unknown;
    if (type == "ethernet")
      itype = InterfaceType::Ethernet;
    else if (type == "loopback")
      itype = InterfaceType::Loopback;
    else if (type == "ppp")
      itype = InterfaceType::PPP;
    if (itype != InterfaceType::Unknown) {
      next = start + 3;
      std::cerr << "[InterfaceToken] parseFromTokens: consumed tokens '" << type
                << "' '" << name << "' starting at " << start
                << " -> type=" << static_cast<int>(itype) << " name='" << name
                << "' next=" << next << "\n";
      return std::make_shared<InterfaceToken>(itype, name);
    }
  }
  // bare interfaces
  std::cerr << "[InterfaceToken] parseFromTokens: bare 'interfaces' at "
            << start << "\n";
  return std::make_shared<InterfaceToken>(InterfaceType::Unknown,
                                          std::string());
}
