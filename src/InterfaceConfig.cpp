/*
 * Copyright (c) 2026, Ravenhammer Research Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "InterfaceConfig.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <optional>

#include "IPAddress.hpp"
#include "IPNetwork.hpp"

#include "VRFConfig.hpp"
#include "SystemConfigurationManager.hpp"

// (no helpers) prepare `ifreq` inline where needed

// Platform-specific `struct ifaddrs` constructor removed. System layer
// (`SystemConfigurationManager`) is responsible for enumerating interfaces
// and constructing `InterfaceConfig` instances.

InterfaceConfig::InterfaceConfig(
    std::string name_, InterfaceType type_, std::unique_ptr<IPNetwork> address_,
    std::vector<std::unique_ptr<IPNetwork>> aliases_,
    std::unique_ptr<VRFConfig> vrf_, std::optional<uint32_t> flags_,
    std::vector<std::string> groups_, std::optional<int> mtu_)
    : name(std::move(name_)), type(type_), address(std::move(address_)),
      aliases(std::move(aliases_)), vrf(std::move(vrf_)), flags(flags_),
      groups(std::move(groups_)), mtu(mtu_) {}

void InterfaceConfig::save() const {
  SystemConfigurationManager scm;
  scm.SaveInterface(*this);
}

// Interface existence helper is provided by `ConfigData::exists`

void InterfaceConfig::destroy() const {
  SystemConfigurationManager scm;
  scm.DestroyInterface(name);
}

void InterfaceConfig::removeAddress(const std::string &addr) const {
  SystemConfigurationManager scm;
  scm.RemoveInterfaceAddress(name, addr);
}

// Copy constructor and assignment defined out-of-line to avoid instantiating
// unique_ptr destructors against incomplete types in headers.
InterfaceConfig::InterfaceConfig(const InterfaceConfig &o) {
  name = o.name;
  type = o.type;
  if (o.address)
    address = o.address->clone();
  else
    address.reset();
  aliases.clear();
  for (const auto &a : o.aliases) {
    if (a)
      aliases.emplace_back(a->clone());
    else
      aliases.emplace_back(nullptr);
  }
  if (o.vrf)
    vrf = std::make_unique<VRFConfig>(*o.vrf);
  else
    vrf.reset();
  flags = o.flags;
  index = o.index;
  groups = o.groups;
  mtu = o.mtu;
  (void)0; // wireless attributes moved to WlanConfig
}

// Static helper: check whether a named interface exists on the system.
bool InterfaceConfig::exists(std::string_view name) {
  SystemConfigurationManager scm;
  return scm.InterfaceExists(name);
}
