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

#include "VxlanTableFormatter.hpp"
#include "VxlanInterfaceConfig.hpp"
#include <string>
#include <vector>

std::string
VxlanTableFormatter::format(const std::vector<VxlanInterfaceConfig> &items) {
  if (items.empty())
    return "No VXLAN interfaces found.\n";

  addColumn("Interface", "Interface", 10, 4, true);
  addColumn("VNI", "VNI", 5, 3, false);
  addColumn("Local", "Local", 5, 6, true);
  addColumn("Remote", "Remote", 5, 6, true);
  addColumn("VRF", "VRF", 5, 3, false);

  for (const auto &vx : items) {
    std::string vniStr = vx.vni ? std::to_string(*vx.vni) : "-";
    std::string localStr = vx.localAddr ? *vx.localAddr : "-";
    std::string remoteStr = vx.remoteAddr ? *vx.remoteAddr : "-";
    std::string vrfStr = vx.vrf ? std::to_string(vx.vrf->table) : "-";

    addRow({vx.name, vniStr, localStr, remoteStr, vrfStr});
  }

  auto out = renderTable(80);
  out += "\n";
  return out;
}
