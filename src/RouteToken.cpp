#include "RouteToken.hpp"

RouteToken::RouteToken(std::string prefix) : prefix_(std::move(prefix)) {}

// textual reconstruction removed from RouteToken

std::vector<std::string> RouteToken::autoComplete(std::string_view) const {
  return {"interface", "next-hop", "blackhole", "reject", "vrf"};
}

std::unique_ptr<Token> RouteToken::clone() const {
  auto r = std::make_unique<RouteToken>(prefix_);
  if (nexthop)
    r->nexthop = nexthop->clone();
  if (interface)
    r->interface = std::make_unique<InterfaceToken>(*interface);
  if (vrf)
    r->vrf = std::make_unique<VRFToken>(*vrf);
  if (blackhole_token)
    r->blackhole_token = std::make_unique<BlackholeToken>(*blackhole_token);
  if (reject_token)
    r->reject_token = std::make_unique<RejectToken>(*reject_token);
  r->blackhole = blackhole;
  r->reject = reject;
  return r;
}

void RouteToken::debugOutput(std::ostream &os) const {
  os << "[parser] parsed route: prefix='" << prefix_ << "'";
  if (nexthop)
    os << " nexthop='" << nexthop->toString() << "'";
  if (vrf)
    os << " vrf='" << vrf->name() << "'";
  if (interface)
    os << " interface='" << interface->name() << "'";
  if (blackhole)
    os << " blackhole=true";
  if (reject)
    os << " reject=true";
  os << '\n';
}

size_t RouteToken::parseOptions(const std::vector<std::string> &tokens,
                                size_t start) {
  size_t j = start;
  while (j < tokens.size()) {
    const auto &opt = tokens[j];
    if (opt == "next-hop" && j + 1 < tokens.size()) {
      const std::string nh = tokens[j + 1];
      std::unique_ptr<IPAddress> addr = IPAddress::fromString(nh);
      if (!addr) {
        auto net = IPNetwork::fromString(nh);
        if (net)
          addr = net->address();
      }
      nexthop = std::move(addr);
      j += 2;
      continue;
    }
    if (opt == "vrf" && j + 1 < tokens.size()) {
      vrf = std::make_unique<VRFToken>(tokens[j + 1]);
      j += 2;
      continue;
    }
    if (opt == "interface" && j + 1 < tokens.size()) {
      interface = std::make_unique<InterfaceToken>(InterfaceType::Unknown,
                                                   tokens[j + 1]);
      j += 2;
      continue;
    }
    if (opt == "blackhole") {
      blackhole = true;
      blackhole_token = std::make_unique<BlackholeToken>();
      ++j;
      continue;
    }
    if (opt == "reject") {
      reject = true;
      reject_token = std::make_unique<RejectToken>();
      ++j;
      continue;
    }
    break;
  }
  return j;
}
