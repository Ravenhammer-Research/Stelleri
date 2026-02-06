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
sudo ./build/netcli -c "show routes"
Routes (FIB: Global)

Destination       Gateway  Interface Flags
----------------- -------- --------- -----
0.0.0.0/0         10.1.0.1 re0.25    UG   
10.1.0.0/18       -        re0.25    C    
10.1.0.21/32      -        lo0       C    
127.0.0.1/32      -        lo0       C    
192.0.0.2/31      -        epair14b  C    
192.0.0.2/32      -        lo0       C    
::/96             ::1      lo0       R    
::1/128           -        lo0       C    
::ffff:0.0.0.0/96 ::1      lo0       R    
fe80::/10         ::1      lo0       R    
fe80::/64         -        lo0       C    
fe80::/64         -        lo1       C    
fe80::1/128       -        lo0       C    
fe80::1/128       -        lo0       C    
ff02::/16         ::1      lo0       R    
```

## Notes

- Many operations that modify system network state require root privileges — use `sudo` when necessary.
- The project is intended as a small example CLI; consult the source under `src/` and headers under `include/` for implementation details.

If you'd like, I can add a command reference or examples of `set` and `show` subcommands next.

### Add route (with sudo)

```text
msi% sudo  ./build/netcli -c "set route add 192.168.0.0/16 blackhole vrf 2"
set route: add (stub saved)
```

### Show routes (vrf 2)

```text
msi% sudo  ./build/netcli -c "show routes vrf 2"
No routes found.
```

