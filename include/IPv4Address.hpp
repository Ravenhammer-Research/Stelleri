/**
 * @file IPv4Address.hpp
 * @brief IPv4 address implementation
 */

#pragma once

#include "IPAddress.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>

/**
 * @brief IPv4 address (32-bit)
 *
 * Stores IPv4 addresses as 32-bit unsigned integers.
 * Supports standard dotted-decimal notation (e.g., "192.168.1.1").
 */
class IPv4Address : public IPAddress {
public:
	explicit IPv4Address(uint32_t v) : IPAddress(static_cast<unsigned __int128>(v)) {}
	AddressFamily family() const override { return AddressFamily::IPv4; }

	std::string toString() const override {
	uint32_t v = static_cast<uint32_t>(value_);
	struct in_addr a4;
	a4.s_addr = htonl(v);
	char buf[INET_ADDRSTRLEN] = {0};
	if (inet_ntop(AF_INET, &a4, buf, sizeof(buf)) == nullptr)
		return std::string();
	return std::string(buf);
}
};
