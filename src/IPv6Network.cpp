#include "IPv6Network.hpp"
#include "IPv6Address.hpp"
#include <memory>
#include <string>

std::unique_ptr<IPAddress> IPv6Network::toAddress() const {
  return std::make_unique<IPv6Address>(networkUint128());
}

std::unique_ptr<IPNetwork> IPv6Network::cloneNetwork() const {
  return std::make_unique<IPv6Network>(networkUint128(), mask());
}

std::string IPv6Network::toString() const {
  auto addr = toAddress();
  return addr->toString() + "/" + std::to_string(mask());
}

std::unique_ptr<IPAddress> IPv6Network::clone() const {
  return std::make_unique<IPv6Network>(networkUint128(), mask());
}
