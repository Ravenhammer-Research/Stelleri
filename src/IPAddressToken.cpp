#include "IPAddress.hpp"
#include "IPv4Address.hpp"
#include "IPv4Network.hpp"
#include "IPv6Address.hpp"
#include "IPv6Network.hpp"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>

/* removed unused maskBits helper to silence compiler warning */

// Note: `toNetwork` removed — network creation should be handled by callers
// using concrete network types or `IPNetwork::fromString` as appropriate.

// IPAddress::fromString is implemented in src/IPAddress.cpp
