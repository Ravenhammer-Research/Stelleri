# NETCONF Refactoring & Implementation TODO List

## Phase 1: Critical Infrastructure Fixes

| ID | Task | Priority | Status |
|----|------|----------|--------|
| 1 | **Fix duplicate include in NetconfExecutor.cpp** (line 45 re-includes `NetconfExecutor.hpp`) | High | ✅ Done |

## Phase 2: Empty Method Implementations (Stub -> Real)

The `NetconfConfigurationManager` has many methods that are stubs returning empty results or doing nothing:

| ID | Category | Methods Needing Implementation |
|----|----------|-------------------------------|
| 2 | **Interface Operations** | ✅ Done - CreateInterface, SaveInterface, DestroyInterface, RemoveInterfaceAddress, InterfaceExists, GetInterfaceAddresses implemented using ietf-interfaces YANG model |
| 3 | **Bridge Operations** | ✅ Done - CreateBridge, SaveBridge, GetBridgeInterfaces; GetBridgeMembers requires custom YANG module for member list |
| 4 | **LAGG Operations** | ✅ Done - CreateLagg, SaveLagg, GetLaggInterfaces; lagg-specific settings need custom YANG |
| 5 | **VLAN Operations** | ✅ Done - SaveVlan, GetVLANInterfaces; VLAN-specific settings need custom YANG |
| 6 | **Tunnel Types** | ✅ Done - CreateTun, SaveTun, GetTunInterfaces; CreateGif, SaveGif, GetGifInterfaces; CreateGre, SaveGre, GetGreInterfaces; CreateOvpn, SaveOvpn, GetOvpnInterfaces; CreateIpsec, SaveIpsec, GetIpsecInterfaces; CreateVxlan, SaveVxlan, GetVxlanInterfaces - all use standard ietf-interfaces with appropriate iana-if-type values |
| 7 | **Specialized Interfaces** | ✅ Done - SixToFour (tunnel type), Pflog (tunnel type), Pfsync (tunnel type), Tap (tunnel type), Wlan (ethernetCsmacd), Epair (ethernetCsmacd) - all use standard ietf-interfaces types |
| 8 | **Carp** | ⚠️ Stub - GetCarpInterfaces, SaveCarp (CARP specific settings need custom YANG) |
| 9 | **Epairs** | ⚠️ Stub - CreateEpair, SaveEpair (epair is FreeBSD-specific, needs custom YANG) |
| 10 | **Routes & VRFs** | ✅ Done - YANG parsers added to RouteConfig and VRFConfig classes; server-side executor can parse from YANG nodes |
| 11 | **Policy** | ⚠️ Stub - All policy methods (require ietf-access-control-list or vendor-specific YANG) |
| 12 | **Neighbor Cache** | ⚠️ Stub - GetArpEntries, SetArpEntry, DeleteArpEntry, GetNdpEntries, etc. using IPNetwork type; ARP/NDP need custom YANG models |

## Phase 3: Architecture & Design

| ID | Task | Priority | Status |
|----|------|----------|--------|
| 15 | **Implement candidate datastore properly** - `editConfig` stages changes in g_candidate, `commit` applies to system via SystemConfigurationManager | High | ✅ Done |
| 16 | **Handle edit-config operations** - Delete/Remove are now parsed from node metadata and queued for commit | High | ✅ Done |
| 17 | **YANG serialization on config classes** - Added `fromYang()` static methods to RouteConfig, VRFConfig, ArpConfig, NdpConfig, PolicyConfig for parsing YANG nodes; added `toLydNode()` instance methods for serializing to YANG | High | ✅ Done |

## Phase 4: Client-Side Enhancements

| ID | Task | Priority | Status |
|----|------|----------|--------|
| 18 | **Add session lifecycle management** - no clean shutdown mechanism for singleton client | Medium | ⚠️ Needs implementation (Client::shutdown() exists but could be improved) |
| 19 | **Improve error handling in RPC calls** - many calls silently return `nullptr` on failure | Low | ⚠️ Could be improved |

---

## Notes

### What about "Decouple from Client singleton"?
The current `NetconfConfigurationManager` uses a global `Client::instance()` singleton to make NETCONF calls. This makes testing difficult because you can't easily mock the client.

Options:
1. **Dependency injection**: Pass a client reference through the constructor
2. **Interface abstraction**: Define a clean interface for NETCONF operations
3. **Keep as-is**: If testing isn't a priority, the singleton pattern works functionally

### Build System Issues
The `#ifndef NC_ENABLED_SSH_TLS` in main.cpp (lines 32-34) is a workaround for libnetconf2 builds that may or may not have SSH/TLS support. This should ideally be handled by CMake/Build configuration rather than preprocessor hacks.

### YANG Model Requirements Summary
| Feature | Standard Model | Custom Model Needed |
|---------|---------------|---------------------|
| Interfaces (Ethernet, LAGG, VLAN, Tun, Gif, etc.) | ✅ ietf-interfaces | ❌ No - uses standard types |
| Routes | ⚠️ ietf-routing (not always available) | ❓ Optional |
| VRFs | ⚠️ ietf-network-instance (not always available) | ❓ Optional |
| Bridges (members list) | ✅ ietf-interfaces (basic) | ✅ Yes - for member list |
| LAGG (protocol, members) | ✅ ietf-interfaces (basic) | ✅ Yes - for protocol/members |
| ARP/NDP entries | ❌ No standard model | ✅ Yes - `stelleri-arp`, `stelleri-ndp` |
| Policy/ACLs | ⚠️ ietf-access-control-list (experimental) | ✅ Recommended |
