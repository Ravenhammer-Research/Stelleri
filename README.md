# netcli

netcli is a small command-line network interface management utility. It provides a simple interactive shell for showing and modifying network interface configuration (listing interfaces, showing details by type/group, and adding/removing addresses).

Prerequisites
- C++ toolchain (g++/clang++)
- CMake 3.13 or newer
- Make

Build
1. Configure and generate the build files (out-of-source build recommended):

	cmake -S . -B build

2. Build the project (example uses 2 parallel jobs):

	cmake --build build -- -j2

This produces the `netcli` executable at `build/netcli`.

Run
Launch the interactive CLI:

	./build/netcli

The prompt is `net>` and supports commands such as `show interface`, `delete interface`, `set`, and others.


Usage examples
Below is the exact CLI session transcript you provided demonstrating build, run, queries, and address removal (including the need for elevated privileges):

```text
msi%  cmake --build build -- -j2
[  1%] Building CXX object CMakeFiles/netcli.dir/src/InterfaceToken.cpp.o
[  3%] Linking CXX executable netcli
# netcli

## Overview

netcli is a small command-line network interface management utility. It provides a simple interactive shell for showing and modifying network interface configuration (listing interfaces, showing details by type/group, and adding/removing addresses).

## Prerequisites

- C++ toolchain (g++/clang++)
- CMake 3.13 or newer
- Make

## Building

1. Configure and generate the build files (out-of-source build recommended):

```bash
cmake -S . -B build
```

2. Build the project (example uses 2 parallel jobs):

```bash
cmake --build build -- -j2
```

This produces the `netcli` executable at `build/netcli`.

## Running

Launch the interactive CLI:

```bash
./build/netcli
```

The prompt is `net>` and supports commands such as `show interface`, `delete interface`, `set`, and others.

## Usage examples

### Build output

```text
msi%  cmake --build build -- -j2
[  1%] Building CXX object CMakeFiles/netcli.dir/src/InterfaceToken.cpp.o
[  3%] Linking CXX executable netcli
[100%] Built target netcli
```

### Start CLI

```text
msi% ./build/netcli
```

### Show all interfaces

```text
net> show interface
Flags: U=UP, B=BROADCAST, D=DEBUG, L=LOOPBACK, P=POINTOPOINT,
	   e=NEEDSEPOCH, R=RUNNING, N=NOARP, O=PROMISC, p=PPROMISC,
	   A=ALLMULTI, a=PALLMULTI, M=MULTICAST, s=SIMPLEX, q=OACTIVE,
	   0/1/2=LINK0/1/2, C=CANTCONFIG, m=MONITOR, x=DYING, z=RENAMING

Index     Interface Group  Type          Address      Status   MTU VRF Flags
--------- --------- ------ ------------- ------------ ------ ----- --- -----
1         re0       -      Ethernet      -            active  1500   0 UBRMs
2         lo0       lo     Loopback      ::1/128      active 16384   0 ULRM 
										 fe80::1/64                         
										 127.0.0.1/8                        
3         re0.25    vlan   VLAN          10.1.0.21/18 active  1500   0 UBRMs
4         bridge0   bridge Bridge        -            down    1500   0 BMs  
5         gif0      gif    GenericTunnel -            down    1280   2 PM   
6         lagg0     lagg   LinkAggregate -            down    1500   2 BMs  
7         lo1       lo     Loopback      fe80::1/64   active 16384   0 ULRM 
8         epair14a  epair  Virtual       -            active  1500   0 BRMs 
9         epair14b  epair  Virtual       192.0.0.2/31 active  1500   2 UBRMs
										 192.0.0.4/31                       
```

### Show interfaces by type: virtual

```text
net> show interface type virtual
peer_a   VRF  MTU Status Flags peer_b   VRF  MTU Status Flags
-------- --- ---- ------ ----- -------- --- ---- ------ -----
epair14a   0 1500   DOWN BRMs  epair14b   2 1500     UP UBRMs
```

### Show interface type: gif

```text
net> show interface type gif
Interface Source    Destination Flags Metric  MTU Groups FIB TunFIB 
--------- --------- ----------- ----- ------ ---- ------ --- ------ 
gif0      192.0.0.0 192.0.0.1   PM         - 1280 all      2      3       
												 gif                      
```

### Show interface group: epair

```text
net> show interface group epair
Flags: U=UP, B=BROADCAST, D=DEBUG, L=LOOPBACK, P=POINTOPOINT,
	   e=NEEDSEPOCH, R=RUNNING, N=NOARP, O=PROMISC, p=PPROMISC,
	   A=ALLMULTI, a=PALLMULTI, M=MULTICAST, s=SIMPLEX, q=OACTIVE,
	   0/1/2=LINK0/1/2, C=CANTCONFIG, m=MONITOR, x=DYING, z=RENAMING

Index     Interface Group Type    Address      Status  MTU VRF Flags
--------- --------- ----- ------- ------------ ------ ---- --- -----
8         epair14a  epair Virtual -            active 1500   0 BRMs 
9         epair14b  epair Virtual 192.0.0.2/31 active 1500   2 UBRMs
								  192.0.0.4/31                      
```

### Delete address (permission denied)

```text
net> delete interface name epair14b inet address 192.0.0.4/31
delete interface: failed to remove address '192.0.0.4/31': Operation not permitted
```

### Delete address (with sudo)

```text
msi% sudo ./build/netcli
net> delete interface name epair14b inet address 192.0.0.4/31
delete interface: removed address '192.0.0.4/31' from 'epair14b'

net> show interface group epair
Flags: U=UP, B=BROADCAST, D=DEBUG, L=LOOPBACK, P=POINTOPOINT,
	   e=NEEDSEPOCH, R=RUNNING, N=NOARP, O=PROMISC, p=PPROMISC,
	   A=ALLMULTI, a=PALLMULTI, M=MULTICAST, s=SIMPLEX, q=OACTIVE,
	   0/1/2=LINK0/1/2, C=CANTCONFIG, m=MONITOR, x=DYING, z=RENAMING

Index     Interface Group Type    Address      Status  MTU VRF Flags
--------- --------- ----- ------- ------------ ------ ---- --- -----
8         epair14a  epair Virtual -            active 1500   0 BRMs 
9         epair14b  epair Virtual 192.0.0.2/31 active 1500   2 UBRMs
```

### Set address (with sudo)

```text
sudo build/netcli -c "set interface name epair14b inet address 192.0.0.8/31"
set interface: updated virtual iface 'epair14b'
```

### Show after set

```text
sudo ./build/netcli -c "show interface"
Flags: U=UP, B=BROADCAST, D=DEBUG, L=LOOPBACK, P=POINTOPOINT,
	   e=NEEDSEPOCH, R=RUNNING, N=NOARP, O=PROMISC, p=PPROMISC,
	   A=ALLMULTI, a=PALLMULTI, M=MULTICAST, s=SIMPLEX, q=OACTIVE,
	   0/1/2=LINK0/1/2, C=CANTCONFIG, m=MONITOR, x=DYING, z=RENAMING

Index     Interface Group  Type          Address      Status   MTU VRF Flags
--------- --------- ------ ------------- ------------ ------ ----- --- -----
1         re0       -      Ethernet      -            active  1500   0 UBRMs
2         lo0       lo     Loopback      ::1/128      active 16384   0 ULRM 
										 fe80::1/64                         
										 127.0.0.1/8                        
3         re0.25    vlan   VLAN          10.1.0.21/18 active  1500   0 UBRMs
4         bridge0   bridge Bridge        -            down    1500   0 BMs  
5         gif0      gif    GenericTunnel -            down    1280   2 PM   
6         lagg0     lagg   LinkAggregate -            down    1500   2 BMs  
7         lo1       lo     Loopback      fe80::1/64   active 16384   0 ULRM 
8         epair14a  epair  Virtual       -            active  1500   0 BRMs 
9         epair14b  epair  Virtual       192.0.0.2/31 active  1500   2 UBRMs
										 192.0.0.8/31                       
```

### Show routes

```text
msi% build/netcli -c "show routes"
Routes (VRF: 0)

Flags: U=up, G=gateway, H=host, S=static, B=blackhole, R=reject

Destination       Gateway  Interface Flags Scope Expire
----------------- -------- --------- ----- ----- ------
0.0.0.0/0         10.1.0.1 re0.25    UGS   -     -     
10.1.0.0/18       link#3   re0.25    U     -     -     
10.1.0.21/32      link#3   lo0       UHS   -     -     
127.0.0.1/32      link#2   lo0       UH    -     -     
192.0.0.2/31      link#9   epair14b  U     -     -     
192.0.0.2/32      link#9   lo0       UHS   -     -     
::/96             ::1      lo0       USR   -     -     
::1/128           link#2   lo0       UHS   -     -     
::ffff:0.0.0.0/96 ::1      lo0       USR   -     -     
fe80::/10         ::1      lo0       USR   -     -     
fe80::/64         link#2   lo0       U     lo0   -     
fe80::/64         link#7   lo1       U     lo1   -     
fe80::1/128       link#2   lo0       UHS   lo0   -     
fe80::1/128       link#7   lo0       UHS   lo1   -     
ff02::/16         ::1      lo0       USR   -     -     

msi% build/netcli -c "show routes vrf 2"
Routes (VRF: 2)

Flags: U=up, G=gateway, H=host, S=static, B=blackhole, R=reject

Destination  Gateway Interface Flags Scope Expire
------------ ------- --------- ----- ----- ------
192.0.0.8/31 link#9  epair14b  U     -     -     
192.0.0.8/32 link#9  lo0       UHS   -     -     
msi%

### Set / Delete routes

```text
msi% sudo build/netcli -c "set route dest 192.168.52.0/32 nexthop reject vrf 2"
set route: 192.168.52.0/32 added
msi% sudo build/netcli -c "delete route dest 192.168.52.0/32 nexthop reject vrf 2"
delete route: 192.168.52.0/32 removed
msi% build/netcli -c "show routes vrf 2"
Routes (VRF: 2)

Flags: U=up, G=gateway, H=host, S=static, B=blackhole, R=reject

Destination  Gateway Interface Flags Scope Expire
------------ ------- --------- ----- ----- ------
192.0.0.8/31 link#9  epair14b  U     -     -     
192.0.0.8/32 link#9  lo0       UHS   -     -     
msi%
```

## Configuration Generation

Generate CLI commands from the current system state:

```bash
build/netcli -g
```

This will output a series of `set` commands that can recreate the current network configuration:

```text
set vrf fibs 255
set interface name lo1 type loopback inet6 address fe80::1/64 mtu 16384 status up
set interface name lo0 type loopback inet6 address ::1/128 mtu 16384 status up
set interface name lo0 type loopback inet6 address fe80::1/64
set interface name lo0 type loopback inet address 127.0.0.1/8
set interface name epair1b type epair status up
set interface name epair14a type epair status down
set interface name epair0b type epair status up
set interface name epair0a type epair status up
set interface name epair1a type epair status up
set interface name epair14b type epair vrf 2 inet address 192.0.0.2/31 status up
set interface name epair14b type epair inet address 192.0.0.8/31
set interface name re0 mtu 1500 status up
set interface name bridge0 type bridge member epair0a member epair1a status up
set interface name lagg0 type lagg vrf 2 member epair0b member epair1b status up
set interface name re0.25 type vlan inet address 10.1.0.21/18 vid 25 parent re0 status up
set interface name gif0 type tunnel vrf 2 source 192.0.0.0 destination 192.0.0.1 tunnel-vrf 3 status down
set route protocol static dest 0.0.0.0/0 nexthop 10.1.0.1 interface re0.25
set route protocol static dest 10.1.0.0/18 nexthop-interface re0.25
set route protocol static dest 10.1.0.21/32 nexthop-interface lo0
set route protocol static dest 127.0.0.1/32 nexthop-interface lo0
set route protocol static dest 192.0.0.2/32 nexthop-interface lo0
set route protocol static dest 192.0.0.2/31 nexthop-interface epair14b
set route protocol static dest ::/96 nexthop reject
set route protocol static dest ::1/128 nexthop-interface lo0
set route protocol static dest ::ffff:0.0.0.0/96 nexthop reject
set route protocol static dest fe80::/10 nexthop reject
set route protocol static dest fe80::/64 nexthop-interface lo0
set route protocol static dest fe80::1/128 nexthop-interface lo0
set route protocol static dest fe80::/64 nexthop-interface lo1
set route protocol static dest fe80::1/128 nexthop-interface lo0
set route protocol static dest ff02::/16 nexthop reject
```
