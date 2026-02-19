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

/**
 * @file IPv6Flags.hpp
 * @brief Portable enum mirrors for FreeBSD per-address IPv6 flags (IN6_IFF_*)
 *        and per-interface ND6 flags (ND6_IFF_*).
 */

#pragma once

#include <cstdint>

/// Per-address IPv6 flags (mirrors IN6_IFF_* from <netinet6/in6_var.h>)
enum class In6AddrFlag : uint32_t {
  Anycast = 0x01,
  Tentative = 0x02,
  Duplicated = 0x04,
  Detached = 0x08,
  Deprecated = 0x10,
  NoDad = 0x20,
  Autoconf = 0x40,
  Temporary = 0x80,
  PreferSource = 0x0100,
};

/// Per-interface ND6 flags (mirrors ND6_IFF_* from <netinet6/nd6.h>)
enum class Nd6Flag : uint32_t {
  PerformNud = 0x1,
  AcceptRtadv = 0x2,
  IfDisabled = 0x8,
  DontSetIfroute = 0x10,
  AutoLinklocal = 0x20,
  NoRadr = 0x40,
  NoPreferIface = 0x80,
  NoDad = 0x100,
  Ipv6Only = 0x200,
};

inline bool hasIn6(uint32_t flags, In6AddrFlag bit) {
  return (flags & static_cast<uint32_t>(bit)) != 0;
}

inline bool hasNd6(uint32_t flags, Nd6Flag bit) {
  return (flags & static_cast<uint32_t>(bit)) != 0;
}
