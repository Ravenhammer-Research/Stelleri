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

#include "ConfigurationManager.hpp"
#include "PolicyToken.hpp"
#include <iostream>

namespace netcli {

  void executeDeletePolicy(const PolicyToken &tok, ConfigurationManager *mgr) {
    if (!mgr) {
      std::cout << "No ConfigurationManager provided\n";
      return;
    }

    if (!tok.acl_id) {
      std::cout << "Error: access-list number is required\n";
      return;
    }

    PolicyConfig pc;
    pc.policy_type = PolicyConfig::Type::AccessList;
    pc.access_list.id = *tok.acl_id;

    // If a specific rule sequence was given, only delete that rule
    if (tok.rule_seq) {
      PolicyAccessListRule rule;
      rule.seq = *tok.rule_seq;
      pc.access_list.rules.push_back(std::move(rule));
    }

    try {
      pc.destroy(*mgr);
      std::cout << "policy access-list " << *tok.acl_id << " deleted\n";
    } catch (const std::exception &e) {
      std::cout << "policy delete failed: " << e.what() << "\n";
    }
  }
} // namespace netcli
