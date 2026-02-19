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

#include "EpairInterfaceConfig.hpp"
#include "ConfigurationManager.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

EpairInterfaceConfig::EpairInterfaceConfig(const InterfaceConfig &base)
    : InterfaceConfig(base) {}

EpairInterfaceConfig::EpairInterfaceConfig(const InterfaceConfig &base,
                                           std::optional<std::string> peer_,
                                           std::optional<int> rdomain_,
                                           bool promiscuous_)
    : EpairInterfaceConfig(base) {
  peer = peer_;
  rdomain = rdomain_;
  promiscuous = promiscuous_;
}

void EpairInterfaceConfig::save(ConfigurationManager &mgr) const {
  mgr.SaveEpair(*this);
}

void EpairInterfaceConfig::create(ConfigurationManager &mgr) const {
  mgr.CreateEpair(name);
}
