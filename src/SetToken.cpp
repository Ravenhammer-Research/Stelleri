#include "SetToken.hpp"

std::vector<std::string> SetToken::autoComplete(std::string_view) const {
	return {"interfaces", "route", "vrf", "protocols"};
}

std::unique_ptr<Token> SetToken::clone() const { return std::make_unique<SetToken>(*this); }
