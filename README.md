# NetCLI - FreeBSD Network Configuration Tool

A VyOS-style command-line interface for FreeBSD network configuration. NetCLI provides a modern, structured approach to managing network interfaces, routes, and VRFs (Forwarding Information Bases) using native FreeBSD system calls.

## Features

### Implemented Functionality

- ✅ **Interface Management**
  - Query interface status, addresses, MTU, flags
  - Configure existing interfaces (IP addresses, MTU, VRF assignment)
  - Create virtual interfaces (loopback, bridge, VLAN, LAGG, tunnel/GIF, epair)
  - Specialized formatters for bridge, LAGG, VLAN, tunnel, and virtual (epair) interfaces
  - Detailed per-member configuration for bridges (STP, priority, path cost, edge ports)
  - Full bridge STP tuning (priority, hello time, forward delay, max age, aging time)
  - LAGG configuration with hash policy, LACP rate, minimum links
  - VLAN configuration with PCP (Priority Code Point)
  - Tunnel configuration with source, destination, tunnel-vrf (FIB), TTL, TOS

- ✅ **Routing**
  - Display kernel routing tables with VRF context
  - Add/delete static routes with protocols static syntax
  - Support for next-hop, interface, blackhole, and reject routes
  - IPv4 and IPv6 support
  - VRF-aware routing

- ✅ **VRF Support**
  - VRF configuration via FreeBSD FIB (Forwarding Information Base)
  - Route table isolation by VRF
  - Tunnel-vrf (FIB) for tunnel interface routing lookups
  - Note: FIB tables are permanent kernel structures and cannot be deleted

- ✅ **CLI Features**
  - Command history persistence (`~/.netcli_history`)
  - Interactive and one-shot command modes
  - Token-based parsing with quoted string support
  - Both `interface` and `interfaces` syntax supported
  - Detailed error messages
  - Specialized table formatters for different interface types

## Requirements

- **OS**: FreeBSD (tested on FreeBSD 14+)
- **Compiler**: C++26 compatible compiler (clang++ 16+)
- **Build System**: CMake 3.25+
- **Privileges**: Root access required for configuration changes

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
sudo ./netcli  # Root required for configuration
```

## Usage

### Interactive Mode

```bash
sudo ./netcli
net> show interfaces
net> set interfaces loopback lo1 address 10.255.255.1/32
net> show routes
net> quit
```

### One-Shot Mode

```bash
sudo ./netcli -c "show interfaces"
sudo ./netcli -c "set route protocols static 0.0.0.0/0 next-hop 192.168.1.1"
```

### Command Line Options

```
netcli [-c command] [-i] [-h]
  -c, --command <cmd>   Execute single command and exit
  -i, --interactive     Interactive mode (default)
  -h, --help           Show help message
```

## Command Reference

### Show Commands

Display current network configuration (read-only, no privileges required):

```bash
# Show all interfaces
show interfaces

# Show specific interface by name
show interfaces name re0
show interface name re0

# Show interfaces by type
show interfaces type ethernet
show interfaces type loopback
show interfaces type bridge
show interfaces type vlan
show interfaces type tunnel
show interfaces type epair

# All also work with singular "interface"
show interface type bridge
show interface type lagg

# Show all routes
show routes

# Show specific route
show route 0.0.0.0/0

# Show VRF information
show vrf <name>
```

**Output Example:**
```
net> show routes
Routes (FIB: Global)

Destination         Gateway           Interface   Flags
--------------------------------------------------------
0.0.0.0/0           10.1.0.1          re0.25      UG
10.1.0.0/18         -                 re0.25      C
127.0.0.1/32        -                 lo0         C
192.0.2.0/24        -                 -           R
```

**Route Flags:**
- `UG` = Up, Gateway (normal route)
- `C` = Connected (directly attached network)
- `B` = Blackhole (silently drop packets)
- `R` = Reject (drop packets and send ICMP unreachable)

### Set Commands
set interface ethernet re0 address 192.168.1.10/24

# Configure MTU
set interfaces ethernet re0 mtu 9000

# Assign to VRF
set interfaces ethernet re0 vrf MGMT

# Create and configure bridge
set interface bridge bridge0 member em0 member em1 stp true
set interface bridge bridge0 priority 32768 hello-time 2 forward-delay 15

# Configure bridge with detailed member settings
set interface bridge bridge0 member em0 member em1

# Create and configure LAGG
set interface lagg lagg0 protocol lacp member em0 member em1
set interface lagg lagg0 hash-policy layer2+3 lacp-rate 1 min-links 1

# Create and configure VLAN
set interface vlan vlan100 vlan-id 100 parent em0 pcp 3

# Create and configure GIF tunnel
set interface tunnel gif0 tunnel-source 10.0.0.1 tunnel-destination 10.0.0.2
set interface tunnel gif0 tunnel-vrf 100 address 192.168.100.1/30

# Create epair (virtual ethernet pair)
set interface virtual epair0a

# Multiple attributes
set interfaces loopback lo1 address 10.0.0.1/32 mtu 16384
```

**Supported Interface Types:**
- **Configurable (existing only)**: ethernet, wireless, ppp
- **Creatable**: loopback, bridge, vlan, lagg, tunnel/gif, virtual/epair

**Bridge Configuration Parameters:**
- `stp` - Enable/disable Spanning Tree Protocol
- `priority` - Bridge priority (0-65535, default 32768)
- `hello-time` - STP hello time in seconds (1-10, default 2)
- `forward-delay` - STP forward delay in seconds (4-30, default 15)
- `max-age` - STP max age in seconds (6-40, default 20)
- `aging-time` - MAC address aging time in seconds (10-1000000)
- `max-addresses` - Maximum MAC addresses in cache

**LAGG Configuration Parameters:**
- `protocol` - LAGG protocol: lacp, failover, loadbalance
- `hash-policy` - Load balancing hash: layer2, layer2+3, layer3+4
- `lacp-rate` - LACP rate: 0=slow (30s), 1=fast (1s)
- `min-links` - Minimum number of active links

**VLAN Configuration Parameters:**
- `vlan-id` - VLAN ID (1-4094)
- `parent` - Parent interface name
- `pcp` - Priority Code Point (0-7)

**Tunnel Configuration Parameters:**
- `tunnel-source` - Tunnel source IP address
- `tunnel-destination` - Tunnel destination IP address
- `tunnel-vrf` - FIB ID for tunnel routing (0-65535)

# Multiple attributes
set interfaces loopback lo1 address 10.0.0.1/32 mtu 16384 description "Management"
```

**Supported Interface Types:**
- **Configurable (existing only)**: ethernet, wireless, ppp
- **Creatable**: loopback, bridge, vlan, tunnel, virtual

**Note**: Physical interfaces must already exist in the system. You cannot create `eth0` via CLI if the hardware doesn't exist.

#### Routes

```bash
# Default route
set route protocols static 0.0.0.0/0 next-hop 192.168.1.1

# Static route with interface
set route protocols static 10.0.0.0/8 interface lo0

# Blackhole route (silently drop packets)
set route protocols static 192.0.2.0/24 blackhole

# Reject route (drop packets, send ICMP unreachable)
set route protocols static 198.51.100.0/24 reject

# Route in specific VRF
set route protocols static 0.0.0.0/0 next-hop 10.1.1.1 vrf MGMT
```

#### VRF (Forwarding Information Base)

``# FreeBSD Integration Details

### Interface Configuration

NetCLI uses standard FreeBSD network ioctls:

```c
SIOCSIFADDR    - Set interface IP address
SIOCSIFNETMASK - Set network mask
SIOCSIFMTU     - Set MTU
SIOCSIFFLAGS   - Bring interface up/down
SIOCGIFFLAGS   - Query interface flags
SIOCIFCREATE   - Create virtual interfaces
```

**Bridge Configuration ioctls:**
```c
BRDGADD        - Add member to bridge
BRDGSIFFLGS    - Set STP flags on member
BRDGSPRI       - Set bridge priority
BRDGSHT        - Set hello time
BRDGSFD        - Set forward delay
BRDGSMA        - Set max age
BRDGSTO        - Set aging time
BRDGSCACHE     - Set cache size
BRDGSIFPRIO    - Set member priority
BRDGSIFCOST    - Set member path cost
```

**LAGG Configuration ioctls:**
```c
SIOCSLAGG      - Set LAGG protocol
SIOCSLAGGPORT  - Add member port to LAGG
```

**VLAN Configuration ioctls:**
```c
SIOCSETVLAN    - Set VLAN tag and parent
```

**Tunnel Configuration ioctls:**
```c
SIOCSIFPHYADDR - Set tunnel endpoints
SIOCSIFFIB     - Set tunnel FIB/VRF
```

**Supported Interface Types:**
- **Physical**: ethernet, wireless, ppp (configure existing only)
- **Virtual**: loopback, bridge, vlan, lagg, tunnel/gif, epair
```

**Note**: VRF/FIB tables cannot be deleted as they are permanent kernel structures.

## Architectureimplemented for bridge, VLAN, LAGG, tunnel/GIF, epair
2. **IPv6 Routing**: Fully implemented via routing sockets
3. **VRF Display**: Show VRF displays name and table number
4. **Rollback**: No configuration rollback/commit mechanism
5. **Validation**: Limited validation before system calls
6. **Bridge Member Details**: Per-member STP configuration supported with detailed flags
7. **LAGG Hash Policy**: Configuration parsed but may require additional sysctl setting
User Input
    ↓
Tokenizer (handles quotes, escapes)
    ↓
Parser (builds token chains)
    ↓
CLI (executes commands) (SetCommand, DeleteCommand, ShowCommand)
  *Token.hpp      Configuration token classes (InterfaceToken, RouteToken, VRFToken, etc.)
  ConfigData.hpp  Data structures
  IP*.hpp         IP address hierarchy
  *Config.hpp     Configuration structures (BridgeInterfaceConfig, LaggConfig, etc.)
  *Formatter.hpp  Table formatter classes
  InterfaceFlags.hpp  Interface flag definitions

src/              Implementation files
  Parser.cpp      Command parsing logic
  CLI.cpp         REPL and command execution
  SystemConfigurationManager.cpp  FreeBSD system integration
  *Token.cpp      Token implementations
  *Formatter.cpp  Table formattero tokens: `["set", "route", "protocols", "static", "0.0.0.0/0", "next-hop", "10.1.1.1"]`
3. Parser creates token chain: SetCommand → RouteToken → ProtocolsToken → StaticToken
4. CLI extracts configuration from tokens
5. SystemConfigurationManager translates to FreeBSD system calls
6. Kernel applies the configuration

### IP Address Classes

Hierarchical IP address representation supporting both IPv4 and IPv6:
- `IPAddress` - Abstract base (128-bit storage)
- `IPv4AddressBridge Interface

```bash
net> set interface bridge bridge0 member em0 member em1 stp true priority 32768
Bridge bridge0 configured successfully

net> show interface type bridge

Bridge Interfaces
================================================================================

Bridge: bridge0
  STP:            enabled
  VLAN Filtering: disabled
  Priority:       32768
  Hello Time:     2s
  Forward Delay:  15s
  Max Age:        20s
  Members:
    - em0
    - em1
```

### Configure LAGG Interface

```bash
net> set interface lagg lagg0 protocol lacp member em2 member em3 hash-policy layer2+3
LAGG lagg0 configured successfully

net> show interface type lagg

LAGG Interfaces
================================================================================

LAGG: lagg0
  Protocol:       LACP
  Hash Policy:    layer2+3
  Members:
    - em2
    - em3
```

### Configure VLAN Interface

```bash
net> set interface vlan vlan100 vlan-id 100 parent em0 pcp 3
VLAN vlan100 configured successfully

net> show interface type vlan

VLAN Interfaces
================================================================================

Interface      VLAN ID   Parent         PCP     Address             MTU     
----------------------------------------------------------------------------
vlan100        100       em0            3       -                   -
```c
SIOCSIFADDR    - Set interface IP address
SIOCSIFNETMASK - Set network mask
SIOCSIFMTU     - Set MTU
SIOCSIFFLAGS   - Bring interface up/down
SIOCGIFFLAGS   - Query interface flags
- Specialized table formatters for each interface type
- Comprehensive bridge, LAGG, VLAN, tunnel, and virtual interface support
```

**Supported Interface Types:**
- **Physical**: ethernet, wireless, ppp (configure existing only)
- **Virtual**: loopback, bridge, vlan, tunnel (can be created)

### Routing

Routes are managed via PF_ROUTE socket with routing messages:
- `RTM_ADD` - Add route to kernel routing table
- `RTM_DELETE` - Remove route from kernel routing table

**Route Flags:**
- `RTF_UP` - Route is active
- `RTF_GATEWAY` - Route uses gateway
- `RTF_STATIC` - Manually configured route
- `RTF_BLACKHOLE` - Silently discard packets
- `RTF_REJECT` - Reject route with ICMP unreachable

### VRF / FIB

FreeBSD uses numbered FIB (Forwarding Information Base) tables for VRF-like functionality:
- FIB tables are kernel routing tables (0-65535)
- Default FIB is 0 (global routing table)
- Use `setfib` command to run processes in specific FIB
- FIB tables are permanent and cannot be deleted

## Security & Permissions

**Root Required For:**
- Setting interface addresses/MTU
- Modifying routing tables
- Changing interface flags (up/down)

**No Privileges Needed For:**
- Show commands (read-only queries)
- Parsing and validation
- Command history

## Known Limitations

1. **Interface Creation**: Virtual interface creation (bridge, VLAN) parsed but not yet implemented
2. **IPv6 Routing**: Uses routing sockets (more complex than shown in stub)
3. **VRF Display**: Show VRF currently only displays name (needs enhancement)
4. **Rollback**: No configuration rollback/commit mechanism
5. **Validation**: Limited validation before system calls

## Development

### File Structure

```
include/          Header files
  Token.hpp       Abstract token base class
  *Command.hpp    Command token classes
  *Token.hpp      Configuration token classes
  ConfigData.hpp  Data structures
  IP*.hpp         IP address hierarchy

src/              Implementation files
  Parser.cpp      Command parsing logic
  CLI.cpp         REPL and command execution
  SystemConfigurationManager.cpp  FreeBSD system integration
  *Token.cpp      Token implementations
  IP*.cpp         IP address implementations
```

### Adding New Commands

1. Create token class inheriting from `Token`
2. Implement `toString()`, `autoComplete()`, `clone()`
3. Add parsing logic in `Parser::parse()`
4. Add execution logic in `CLI::processLine()` or token class
5. Update `SystemConfigurationManager` for system integration

## Examples

### Configure Loopback Interface

```bash
net> set interfaces loopback lo1 address 10.255.255.1/32
Interface lo1 configured successfully

net> show interfaces loopback lo1
Interface  Type      Address           Status  MTU    VRF  Flags
-----------------------------------------------------------------
lo1        loopback  10.255.255.1/32   active  16384  -    ULR
```

### Add Static Routes

```bash
net> set route protocols static 10.0.0.0/8 next-hop 192.168.1.254
Route 10.0.0.0/8 configured successfully

net> set route protocols static 192.0.2.0/24 blackhole
Route 192.0.2.0/24 configured successfully

net> set route protocols static 198.51.100.0/24 reject
Route 198.51.100.0/24 configured successfully

net> show routes
Routes (FIB: Global)

Destination         Gateway           Interface   Flags
--------------------------------------------------------
0.0.0.0/0           192.168.1.1       re0         UG
10.0.0.0/8          192.168.1.254     re0         UG
192.0.2.0/24        -                 -           B
198.51.100.0/24     -                 -           R
```

### Error Handling

```bash
# Attempt to create non-existent physical interface
net> set interfaces ethernet eth99 address 10.0.0.1/24
Error configuring interface: Cannot create interface 'eth99': 
Physical/hardware interfaces cannot be created via CLI. 
Interface does not exist or is not attached.
```

## History

Command history is automatically saved to `~/.netcli_history`:
- One command per line
- Appended after each successful parse
- Used for session continuity

## License

See TODO.md for project roadmap and future enhancements.

## Contributing

This is part of the Stelleri NetCLI project. The architecture supports:
- Additional command types via Token subclasses
- Alternative backends via ConfigurationManager interface
- Extended functionality (BGP, OSPF, etc.) via ProtocolsToken

---

**Note**: This tool directly modifies kernel network configuration. Always test in a safe environment before production use.
