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

#include "SingleVlanSummaryFormatter.hpp"
#include "SingleInterfaceSummaryFormatter.hpp"
#include "VlanProto.hpp"
#include <sstream>

std::string
SingleVlanSummaryFormatter::format(const VlanInterfaceConfig &vlan) const {
  SingleInterfaceSummaryFormatter base;
  std::string out = base.format(vlan);

  std::ostringstream oss;
  if (vlan.id != 0)
    oss << "VLAN ID:   " << vlan.id << "\n";
  if (vlan.parent)
    oss << "Parent:    " << *vlan.parent << "\n";
  if (vlan.proto) {
    const char *p = "unknown";
    switch (*vlan.proto) {
    case VLANProto::DOT1Q:
      p = "802.1Q";
      break;
    case VLANProto::DOT1AD:
      p = "802.1ad";
      break;
    default:
      break;
    }
    oss << "Proto:     " << p << "\n";
  }
  if (vlan.pcp)
    oss << "PCP:       " << static_cast<int>(*vlan.pcp) << "\n";

  out += oss.str();
  return out;
}
