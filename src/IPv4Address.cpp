#include "IPv4Address.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>

std::string IPv4Address::toString() const {
  char buf[INET_ADDRSTRLEN] = {0};
  in_addr a4;
  a4.s_addr = htonl(asUint32());
  if (inet_ntop(AF_INET, &a4, buf, sizeof(buf)) == nullptr)
    return std::string();
  return std::string(buf);
}

std::unique_ptr<IPAddress> IPv4Address::clone() const {
  return std::make_unique<IPv4Address>(asUint32());
}
