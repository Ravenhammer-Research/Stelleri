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

#include "SingleVxlanSummaryFormatter.hpp"
#include "SingleInterfaceSummaryFormatter.hpp"
#include <sstream>

std::string
SingleVxlanSummaryFormatter::format(const VxlanInterfaceConfig &vxlan) const {
  SingleInterfaceSummaryFormatter base;
  std::string out = base.format(vxlan);

  std::ostringstream oss;
  if (vxlan.vni)
    oss << "VNI:       " << *vxlan.vni << "\n";
  if (vxlan.localAddr)
    oss << "Local:     " << *vxlan.localAddr << "\n";
  if (vxlan.remoteAddr)
    oss << "Remote:    " << *vxlan.remoteAddr << "\n";
  if (vxlan.localPort)
    oss << "LocalPort: " << *vxlan.localPort << "\n";
  if (vxlan.remotePort)
    oss << "RemotePort:" << *vxlan.remotePort << "\n";
  if (vxlan.ttl)
    oss << "TTL:       " << static_cast<int>(*vxlan.ttl) << "\n";
  if (vxlan.learn)
    oss << "Learn:     " << (*vxlan.learn ? "ON" : "OFF") << "\n";
  if (vxlan.multicastIf)
    oss << "McastIf:   " << *vxlan.multicastIf << "\n";
  if (vxlan.portMin || vxlan.portMax) {
    oss << "PortRange: ";
    if (vxlan.portMin)
      oss << *vxlan.portMin;
    oss << "-";
    if (vxlan.portMax)
      oss << *vxlan.portMax;
    oss << "\n";
  }
  if (vxlan.ftableTimeout)
    oss << "FTblTmout: " << *vxlan.ftableTimeout << "s\n";
  if (vxlan.ftableMax)
    oss << "FTblMax:   " << *vxlan.ftableMax << "\n";

  out += oss.str();
  return out;
}
