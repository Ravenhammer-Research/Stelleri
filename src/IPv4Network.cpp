#include "IPv4Network.hpp"
#include "IPv4Address.hpp"
#include <memory>
#include <string>

std::unique_ptr<IPAddress> IPv4Network::toAddress() const {
  return std::make_unique<IPv4Address>(networkUint32());
}

std::unique_ptr<IPNetwork> IPv4Network::cloneNetwork() const {
  return std::make_unique<IPv4Network>(networkUint32(), mask());
}

std::string IPv4Network::toString() const {
  auto addr = toAddress();
  return addr->toString() + "/" + std::to_string(mask());
}

std::unique_ptr<IPAddress> IPv4Network::clone() const {
  return std::make_unique<IPv4Network>(networkUint32(), mask());
}
