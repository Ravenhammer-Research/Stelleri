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

#include "SingleBridgeSummaryFormatter.hpp"
#include "SingleInterfaceSummaryFormatter.hpp"
#include <sstream>

std::string
SingleBridgeSummaryFormatter::format(const BridgeInterfaceConfig &br) const {
  SingleInterfaceSummaryFormatter base;
  std::string out = base.format(br);

  std::ostringstream oss;
  oss << "STP:       " << (br.stp ? "ON" : "OFF") << "\n";
  if (br.stp_protocol) {
    const char *p = "STP";
    if (*br.stp_protocol == 2)
      p = "RSTP";
    oss << "STP Proto: " << p << "\n";
  }
  if (br.priority)
    oss << "Priority:  " << *br.priority << "\n";
  if (br.hello_time)
    oss << "Hello:     " << *br.hello_time << "s\n";
  if (br.forward_delay)
    oss << "FwdDelay:  " << *br.forward_delay << "s\n";
  if (br.max_age)
    oss << "MaxAge:    " << *br.max_age << "s\n";
  if (br.aging_time)
    oss << "AgingTime: " << *br.aging_time << "s\n";
  if (br.holdcount)
    oss << "HoldCnt:   " << *br.holdcount << "\n";
  if (br.max_addresses)
    oss << "MaxAddrs:  " << *br.max_addresses << "\n";
  oss << "VLAN Filt: " << (br.vlanFiltering ? "ON" : "OFF") << "\n";
  if (br.default_pvid)
    oss << "Def PVID:  " << *br.default_pvid << "\n";
  if (!br.members.empty()) {
    oss << "Members:   ";
    for (size_t i = 0; i < br.members.size(); ++i) {
      if (i)
        oss << ", ";
      oss << br.members[i];
    }
    oss << "\n";
  }

  out += oss.str();
  return out;
}
