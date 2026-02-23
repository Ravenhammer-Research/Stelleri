/* TargetToken.cpp -- parse and execute `set target ...` */

#if defined(STELLERI_NETCONF) && STELLERI_NETCONF == 1

#include "TargetToken.hpp"
#include "Client.hpp"
#include <cstdlib>
#include <iostream>

std::shared_ptr<TargetToken>
TargetToken::parseFromTokens(const std::vector<std::string> &tokens,
                             size_t start, size_t &next) {
  // start points at "target"
  size_t cur = start;
  if (cur >= tokens.size() || tokens[cur] != "target")
    return nullptr;
  ++cur;
  if (cur >= tokens.size()) {
    next = cur;
    return nullptr;
  }

  auto tok = std::make_shared<TargetToken>();
  const std::string &mode = tokens[cur++];
  if (mode == "unix") {
    tok->type = Type::Unix;
    if (cur < tokens.size()) {
      tok->path = tokens[cur++];
    } else {
      return nullptr;
    }
  } else if (mode == "ssh") {
    tok->type = Type::Ssh;
    if (cur < tokens.size()) {
      tok->host = tokens[cur++];
    } else {
      return nullptr;
    }
    // optional port keyword
    if (cur + 1 < tokens.size() && tokens[cur] == "port") {
      tok->port = static_cast<uint16_t>(std::stoi(tokens[cur + 1]));
      cur += 2;
    }
  } else if (mode == "tls") {
    tok->type = Type::Tls;
    if (cur < tokens.size()) {
      tok->host = tokens[cur++];
    } else {
      return nullptr;
    }
    if (cur + 1 < tokens.size() && tokens[cur] == "port") {
      tok->port = static_cast<uint16_t>(std::stoi(tokens[cur + 1]));
      cur += 2;
    }
  } else {
    return nullptr;
  }

  next = cur;
  return tok;
}

void TargetToken::executeSet(ConfigurationManager *) const {
  switch (type) {
  case Type::Unix:
    if (path.empty()) {
      std::cerr << "target: missing unix socket path\n";
      return;
    }
    if (!Client::init_unix(path)) {
      std::cerr << "target: failed to connect to unix socket: " << path << "\n";
    }
    break;
  case Type::Ssh:
#ifdef NC_ENABLED_SSH_TLS
  {
    uint16_t p = port.value_or(NC_PORT_SSH);
    if (!Client::init_ssh(host, p))
      std::cerr << "target: failed to init ssh client: " << host << ":" << p
                << "\n";
  }
#else
    std::cerr << "target: ssh not supported in this build\n";
#endif
  break;
  case Type::Tls:
#ifdef NC_ENABLED_SSH_TLS
  {
    uint16_t p = port.value_or(NC_PORT_TLS);
    if (!Client::init_tls(host, p))
      std::cerr << "target: failed to init tls client: " << host << ":" << p
                << "\n";
  }
#else
    std::cerr << "target: tls not supported in this build\n";
#endif
  break;
  default:
    std::cerr << "target: unknown target type\n";
  }
}

#endif