➜  ~ sudo ./net -g               
set vrf fibnum 1024
set interface name lo128 type loopback vrf 128 mtu 16384 address fe80::1/64 group lo
set interface name lo26 type loopback vrf 26 mtu 16384 address fe80::1/64 group lo
set interface name lo16 type loopback vrf 16 mtu 16384 address fe80::1/64 group lo
set interface name lo16 type loopback vrf 16 mtu 16384 address 127.0.0.12/32 group lo
set interface name lo0 type loopback vrf 0 mtu 16384 address ::1/128 group lo
set interface name lo0 type loopback vrf 0 mtu 16384 address fe80::1/64 group lo
set interface name lo0 type loopback vrf 0 mtu 16384 address 127.0.0.1/8 group lo
set interface name lo80 type loopback vrf 80 mtu 16384 address fe80::1/64 group lo
set interface name lo80 type loopback vrf 80 mtu 16384 address 127.0.1.3/8 group lo
set interface name lo81 type loopback vrf 81 mtu 16384 address fe80::1/64 group lo
set interface name lo81 type loopback vrf 81 mtu 16384 address 127.0.1.4/8 group lo
set interface name lo53 type loopback vrf 53 mtu 16384 address fe80::1/64 group lo
set interface name lo53 type loopback vrf 53 mtu 16384 address 127.0.1.2/8 group lo
set interface name lo255 type loopback vrf 255 mtu 16384 address fe80::1/64 group lo
set interface name lo180 type loopback vrf 180 mtu 16384 address fe80::1/64 group lo
set interface name lo180 type loopback vrf 180 mtu 16384 address 2001:470:e845::255:65/128 group lo
set interface name lo123 type loopback vrf 0 mtu 16384 group lo
set interface name lo24 type loopback vrf 0 mtu 16384 group lo
set interface name lo1 type loopback vrf 1 mtu 16384 address fe80::1/64 group lo
set interface name lo1 type loopback vrf 1 mtu 16384 address 127.0.0.2/32 group lo
set interface name lo20 type loopback vrf 20 mtu 16384 address fe80::1/64 group lo
set interface name lo20 type loopback vrf 20 mtu 16384 address 127.0.0.3/32 group lo
set interface name lo25 type loopback vrf 25 mtu 16384 address fe80::1/64 group lo
set interface name lo25 type loopback vrf 25 mtu 16384 address 127.0.0.4/32 group lo
set interface name lo48 type loopback vrf 48 mtu 16384 address fe80::1/64 group lo
set interface name lo27 type loopback vrf 27 mtu 16384 address fe80::1/64 group lo
set interface name epair24b type epair vrf 243 mtu 1500 address 192.0.0.45/31 group epair
set interface name epair24b type epair vrf 243 mtu 1500 address fe80::d4:41ff:fe08:fa0b/64 group epair
set interface name epair24b type epair vrf 243 mtu 1500 address fcfc:243::192:0:0:45/64 group epair
set interface name epair128a type epair vrf 0 mtu 1500 group epair
set interface name epair3b type epair vrf 30 mtu 1500 address 192.0.0.17/31 group epair
set interface name epair3b type epair vrf 30 mtu 1500 address fe80::fe:bff:fe4e:150b/64 group epair
set interface name epair3b type epair vrf 30 mtu 1500 address fcfc:30::192:0:0:17/64 group epair
set interface name epair22b type epair vrf 56 mtu 1500 address 192.0.0.41/31 group epair
set interface name epair22b type epair vrf 56 mtu 1500 address fe80::f3:98ff:fe1a:590b/64 group epair
set interface name epair22b type epair vrf 56 mtu 1500 address fcfc:180::192:0:0:41/64 group epair
set interface name epair22a type epair vrf 0 mtu 1500 address 192.0.0.40/31 group epair
set interface name epair22a type epair vrf 0 mtu 1500 address fe80::f3:98ff:fe1a:590a/64 group epair
set interface name epair22a type epair vrf 0 mtu 1500 address fcfc:180::192:0:0:40/64 group epair
set interface name epair9a type epair vrf 0 mtu 1500 address 192.0.0.28/31 group epair
set interface name epair9a type epair vrf 0 mtu 1500 address fe80::6d:85ff:fe3c:450a/64 group epair
set interface name epair9a type epair vrf 0 mtu 1500 address fcfc:48::192:0:0:28/64 group epair
set interface name epair21b type epair vrf 28 mtu 1500 address 192.0.0.39/31 group epair
set interface name epair21b type epair vrf 28 mtu 1500 address fe80::8a:cdff:fe38:9f0b/64 group epair
set interface name epair21b type epair vrf 28 mtu 1500 address fcfc:180::192:0:0:39/64 group epair
set interface name epair21a type epair vrf 0 mtu 1500 address 192.0.0.38/31 group epair
set interface name epair21a type epair vrf 0 mtu 1500 address fe80::8a:cdff:fe38:9f0a/64 group epair
set interface name epair21a type epair vrf 0 mtu 1500 address fcfc:180::192:0:0:38/64 group epair
set interface name epair16a type epair vrf 0 mtu 1500 group epair
set interface name epair20b type epair vrf 16 mtu 1500 address fe80::30:2fff:fe76:950b/64 group epair
set interface name epair20a type epair vrf 16 mtu 1500 address fe80::30:2fff:fe76:950a/64 group epair
set interface name epair20a type epair vrf 16 mtu 1500 address 192.168.32.192/25 group epair
set interface name epair19b type epair vrf 180 mtu 1500 address 192.0.0.37/31 group epair
set interface name epair19b type epair vrf 180 mtu 1500 address fe80::87:1ff:fe11:580b/64 group epair
set interface name epair19b type epair vrf 180 mtu 1500 address fcfc:180::192:0:0:37/64 group epair
set interface name epair1a type epair vrf 0 mtu 1500 address 192.0.0.12/31 group epair
set interface name epair1a type epair vrf 0 mtu 1500 address fe80::97:74ff:fe8c:e60a/64 group epair
set interface name epair1a type epair vrf 0 mtu 1500 address fcfc:18::192:0:0:12/64 group epair
set interface name epair13a type epair vrf 1 mtu 1500 address fe80::37:58ff:fec9:ed0a/64 group epair
set interface name epair13a type epair vrf 1 mtu 1500 address 10.255.255.128/24 group epair
set interface name epair1b type epair vrf 18 mtu 1500 address 192.0.0.13/31 group epair
set interface name epair1b type epair vrf 18 mtu 1500 address fe80::97:74ff:fe8c:e60b/64 group epair
set interface name epair1b type epair vrf 18 mtu 1500 address fcfc:18::192:0:0:13/64 group epair
set interface name epair19a type epair vrf 0 mtu 1500 address 192.0.0.36/31 group epair
set interface name epair19a type epair vrf 0 mtu 1500 address fe80::87:1ff:fe11:580a/64 group epair
set interface name epair19a type epair vrf 0 mtu 1500 address fcfc:180::192:0:0:36/64 group epair
set interface name epair23b type epair vrf 242 mtu 1500 address 192.0.0.43/31 group epair
set interface name epair23b type epair vrf 242 mtu 1500 address fe80::67:4fff:fee1:5e0b/64 group epair
set interface name epair23b type epair vrf 242 mtu 1500 address fcfc:242::192:0:0:43/64 group epair
set interface name epair15b type epair vrf 25 mtu 1500 address fe80::57:35ff:feda:b20b/64 group epair
set interface name epair16b type epair vrf 0 mtu 1500 group epair
set interface name epair10a type epair vrf 0 mtu 1500 address 192.0.0.30/31 group epair
set interface name epair10a type epair vrf 0 mtu 1500 address fe80::b7:95ff:fe19:90a/64 group epair
set interface name epair10a type epair vrf 0 mtu 1500 address fcfc:255::192:0:0:30/64 group epair
set interface name epair23a type epair vrf 0 mtu 1500 address 192.0.0.42/31 group epair
set interface name epair23a type epair vrf 0 mtu 1500 address fe80::67:4fff:fee1:5e0a/64 group epair
set interface name epair23a type epair vrf 0 mtu 1500 address fcfc:242::192:0:0:42/64 group epair
set interface name epair18a type epair vrf 255 mtu 1500 address fe80::55:43ff:fe8f:a10a/64 group epair
set interface name epair14a type epair vrf 20 mtu 1500 address fe80::b3:16ff:fe82:ce0a/64 group epair
set interface name epair14a type epair vrf 20 mtu 1500 address 10.1.126.255/18 group epair
set interface name epair13b type epair vrf 1 mtu 1500 address fe80::37:58ff:fec9:ed0b/64 group epair
set interface name epair6a type epair vrf 0 mtu 1500 address 192.0.0.22/31 group epair
set interface name epair6a type epair vrf 0 mtu 1500 address fe80::63:c8ff:fe2c:e70a/64 group epair
set interface name epair6a type epair vrf 0 mtu 1500 address fcfc:81::192:0:0:22/64 group epair
set interface name epair18b type epair vrf 255 mtu 1500 address fe80::55:43ff:fe8f:a10b/64 group epair
set interface name epair0a type epair vrf 0 mtu 1500 address 192.0.0.10/31 group epair
set interface name epair0a type epair vrf 0 mtu 1500 address fe80::6a:fff:fe48:5c0a/64 group epair
set interface name epair0a type epair vrf 0 mtu 1500 address fcfc:16::192:0:0:10/64 group epair
set interface name epair7b type epair vrf 123 mtu 1500 address 192.0.0.25/31 group epair
set interface name epair7b type epair vrf 123 mtu 1500 address fe80::82:b9ff:fe75:cc0b/64 group epair
set interface name epair7b type epair vrf 123 mtu 1500 address fcfc:123::192:0:0:25/64 group epair
set interface name epair15a type epair vrf 25 mtu 1500 address fe80::57:35ff:feda:b20a/64 group epair
set interface name epair15a type epair vrf 25 mtu 1500 address 10.1.62.255/18 group epair
set interface name epair5a type epair vrf 0 mtu 1500 address 192.0.0.20/31 group epair
set interface name epair5a type epair vrf 0 mtu 1500 address fe80::8f:13ff:fe1c:d0a/64 group epair
set interface name epair5a type epair vrf 0 mtu 1500 address fcfc:80::192:0:0:20/64 group epair
set interface name epair4b type epair vrf 53 mtu 1500 address 192.0.0.19/31 group epair
set interface name epair4b type epair vrf 53 mtu 1500 address fe80::3a:cbff:feba:280b/64 group epair
set interface name epair4b type epair vrf 53 mtu 1500 address fcfc:53::192:0:0:19/64 group epair
set interface name epair14b type epair vrf 20 mtu 1500 address fe80::b3:16ff:fe82:ce0b/64 group epair
set interface name epair12b type epair vrf 20 mtu 1500 address 192.0.0.35/31 group epair
set interface name epair12b type epair vrf 20 mtu 1500 address fe80::a3:46ff:fef9:4d0b/64 group epair
set interface name epair12b type epair vrf 20 mtu 1500 address fcfc:20::192:0:0:35/64 group epair
set interface name epair11b type epair vrf 1 mtu 1500 address 192.0.0.33/31 group epair
set interface name epair11b type epair vrf 1 mtu 1500 address fe80::53:2ff:feab:c00b/64 group epair
set interface name epair11b type epair vrf 1 mtu 1500 address fcfc:1::192:0:0:33/64 group epair
set interface name epair2a type epair vrf 0 mtu 1500 address 192.0.0.14/31 group epair
set interface name epair2a type epair vrf 0 mtu 1500 address fe80::e3:5dff:fe5e:210a/64 group epair
set interface name epair2a type epair vrf 0 mtu 1500 address fcfc:25::192:0:0:14/64 group epair
set interface name epair6b type epair vrf 81 mtu 1500 address 192.0.0.23/31 group epair
set interface name epair6b type epair vrf 81 mtu 1500 address fe80::63:c8ff:fe2c:e70b/64 group epair
set interface name epair6b type epair vrf 81 mtu 1500 address fcfc:81::192:0:0:23/64 group epair
set interface name epair128b type epair vrf 0 mtu 1500 group epair
set interface name epair9b type epair vrf 48 mtu 1500 address 192.0.0.29/31 group epair
set interface name epair9b type epair vrf 48 mtu 1500 address fe80::6d:85ff:fe3c:450b/64 group epair
set interface name epair9b type epair vrf 48 mtu 1500 address fcfc:48::192:0:0:29/64 group epair
set interface name epair4a type epair vrf 0 mtu 1500 address 192.0.0.18/31 group epair
set interface name epair4a type epair vrf 0 mtu 1500 address fe80::3a:cbff:feba:280a/64 group epair
set interface name epair4a type epair vrf 0 mtu 1500 address fcfc:53::192:0:0:18/64 group epair
set interface name epair24a type epair vrf 0 mtu 1500 address 192.0.0.44/31 group epair
set interface name epair24a type epair vrf 0 mtu 1500 address fe80::d4:41ff:fe08:fa0a/64 group epair
set interface name epair24a type epair vrf 0 mtu 1500 address fcfc:243::192:0:0:44/64 group epair
set interface name epair12a type epair vrf 0 mtu 1500 address 192.0.0.34/31 group epair
set interface name epair12a type epair vrf 0 mtu 1500 address fe80::a3:46ff:fef9:4d0a/64 group epair
set interface name epair12a type epair vrf 0 mtu 1500 address fcfc:20::192:0:0:34/64 group epair
set interface name epair17b type epair vrf 0 mtu 1500 group epair
set interface name epair17a type epair vrf 0 mtu 1500 group epair
set interface name epair7a type epair vrf 0 mtu 1500 address 192.0.0.24/31 group epair
set interface name epair7a type epair vrf 0 mtu 1500 address fe80::82:b9ff:fe75:cc0a/64 group epair
set interface name epair7a type epair vrf 0 mtu 1500 address fcfc:123::192:0:0:24/64 group epair
set interface name epair0b type epair vrf 16 mtu 1500 address 192.0.0.11/31 group epair
set interface name epair0b type epair vrf 16 mtu 1500 address fe80::6a:fff:fe48:5c0b/64 group epair
set interface name epair0b type epair vrf 16 mtu 1500 address fcfc:16::192:0:0:11/64 group epair
set interface name epair2b type epair vrf 25 mtu 1500 address 192.0.0.15/31 group epair
set interface name epair2b type epair vrf 25 mtu 1500 address fe80::e3:5dff:fe5e:210b/64 group epair
set interface name epair2b type epair vrf 25 mtu 1500 address fcfc:25::192:0:0:15/64 group epair
set interface name epair5b type epair vrf 80 mtu 1500 address 192.0.0.21/31 group epair
set interface name epair5b type epair vrf 80 mtu 1500 address fe80::8f:13ff:fe1c:d0b/64 group epair
set interface name epair5b type epair vrf 80 mtu 1500 address fcfc:80::192:0:0:21/64 group epair
set interface name epair10b type epair vrf 255 mtu 1500 address 192.0.0.31/31 group epair
set interface name epair10b type epair vrf 255 mtu 1500 address fe80::b7:95ff:fe19:90b/64 group epair
set interface name epair10b type epair vrf 255 mtu 1500 address fcfc:255::192:0:0:31/64 group epair
set interface name epair11a type epair vrf 0 mtu 1500 address 192.0.0.32/31 group epair
set interface name epair11a type epair vrf 0 mtu 1500 address fe80::53:2ff:feab:c00a/64 group epair
set interface name epair11a type epair vrf 0 mtu 1500 address fcfc:1::192:0:0:32/64 group epair
set interface name epair3a type epair vrf 0 mtu 1500 address 192.0.0.16/31 group epair
set interface name epair3a type epair vrf 0 mtu 1500 address fe80::fe:bff:fe4e:150a/64 group epair
set interface name epair3a type epair vrf 0 mtu 1500 address fcfc:30::192:0:0:16/64 group epair
set interface name re0 vrf 1 mtu 1500
set interface name re1 vrf 1 mtu 1500
set interface name bridge4 type bridge vrf 16 mtu 1500 group bridge member epair20b member lagg0.16
set interface name bridge5 type bridge vrf 0 mtu 1500 group bridge
set interface name bridge3 type bridge vrf 255 mtu 1500 group bridge
set interface name bridge0 type bridge vrf 1 mtu 1500 group bridge member epair13b member lagg0
set interface name bridge1 type bridge vrf 20 mtu 1500 group bridge member epair14b member lagg0.20
set interface name bridge2 type bridge vrf 25 mtu 1500 group bridge member epair15b member lagg0.25
set interface name bridge6 type bridge vrf 0 mtu 1500 group bridge
set interface name lagg0 type lagg vrf 1 mtu 1500 address 10.255.255.254/24 group lagg member re0 member re1
set interface name lagg0 type lagg vrf 1 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group lagg
set interface name lagg0 type lagg vrf 1 mtu 1500 address fcff:ffff::10:255:255:254/64 group lagg
set interface name vlan128.1 type vlan vrf 128 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 1 parent vlan128
set interface name vlan26.3 type vlan vrf 0 mtu 1496 group vlan vid 3 parent vlan26
set interface name vlan26.2 type vlan vrf 0 mtu 1496 group vlan vid 2 parent vlan26
set interface name vlan180.7 type vlan vrf 180 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 7 parent vlan180
set interface name vlan180.6 type vlan vrf 180 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 6 parent vlan180
set interface name vlan180.6 type vlan vrf 180 mtu 1496 address 198.51.100.0/32 group vlan
set interface name vlan180.5 type vlan vrf 180 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 5 parent vlan180
set interface name vlan180.5 type vlan vrf 180 mtu 1496 address 2001:470:e845::23/128 group vlan
set interface name vlan180.3 type vlan vrf 180 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 3 parent vlan180
set interface name vlan180.3 type vlan vrf 180 mtu 1496 address 2001:470:e845::80/128 group vlan
set interface name vlan180.2 type vlan vrf 180 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 2 parent vlan180
set interface name vlan180.2 type vlan vrf 180 mtu 1496 address 2001:470:e845::80:443/128 group vlan
set interface name vlan180.1 type vlan vrf 180 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 1 parent vlan180
set interface name vlan180.1 type vlan vrf 180 mtu 1496 address fcff:180::198:51:100:0/128 group vlan
set interface name vlan81.3 type vlan vrf 81 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 3 parent vlan81
set interface name vlan81.2 type vlan vrf 81 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 2 parent vlan81
set interface name vlan81.2 type vlan vrf 81 mtu 1496 address fcff:81::192:88:99:0/128 group vlan
set interface name vlan80.4 type vlan vrf 80 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 4 parent vlan80
set interface name vlan80.3 type vlan vrf 80 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 3 parent vlan80
set interface name vlan80.3 type vlan vrf 80 mtu 1496 address 2001:470:e845::3128/128 group vlan
set interface name vlan80.1 type vlan vrf 80 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 1 parent vlan80
set interface name vlan80.1 type vlan vrf 80 mtu 1496 address 192.88.99.2/32 group vlan
set interface name vlan53.4 type vlan vrf 53 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 4 parent vlan53
set interface name vlan53.4 type vlan vrf 53 mtu 1496 address 127.0.0.7/32 group vlan
set interface name vlan53.2 type vlan vrf 53 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 2 parent vlan53
set interface name vlan53.2 type vlan vrf 53 mtu 1496 address 203.0.113.53/32 group vlan
set interface name vlan48.4 type vlan vrf 48 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 4 parent vlan48
set interface name vlan48.4 type vlan vrf 48 mtu 1496 address 2001:470:b:56f::/64 group vlan
set interface name vlan48.3 type vlan vrf 48 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 3 parent vlan48
set interface name vlan48.2 type vlan vrf 48 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 2 parent vlan48
set interface name lagg0.123 type vlan vrf 123 mtu 1500 address 203.0.113.123/32 group vlan vid 123 parent lagg0
set interface name lagg0.123 type vlan vrf 123 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan
set interface name lagg0.123 type vlan vrf 123 mtu 1500 address fcff::123/64 group vlan
set interface name lagg0.30 type vlan vrf 30 mtu 1500 address 192.168.72.129/25 group vlan vid 30 parent lagg0
set interface name lagg0.30 type vlan vrf 30 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan
set interface name lagg0.30 type vlan vrf 30 mtu 1500 address fcff:30::192:168:72:129/64 group vlan
set interface name lagg0.26 type vlan vrf 26 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 26 parent lagg0
set interface name vlan81 type vlan vrf 81 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 81 parent lagg0
set interface name lagg0.24 type vlan vrf 24 mtu 1500 address 10.1.128.1/18 group vlan vid 24 parent lagg0
set interface name lagg0.24 type vlan vrf 24 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan
set interface name lagg0.24 type vlan vrf 24 mtu 1500 address fcff:24::10:1:128:1/64 group vlan
set interface name vlan128 type vlan vrf 128 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 128 parent lagg0
set interface name vlan80 type vlan vrf 80 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 80 parent lagg0
set interface name vlan28 type vlan vrf 0 mtu 1500 group vlan vid 0 parent 
set interface name vlan27 type vlan vrf 0 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 0 parent 
set interface name vlan26 type vlan vrf 0 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 0 parent 
set interface name vlan26.1 type vlan vrf 26 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 1 parent vlan26
set interface name lagg0.25 type vlan vrf 25 mtu 1500 address 10.1.0.1/18 group vlan vid 25 parent lagg0
set interface name lagg0.25 type vlan vrf 25 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan
set interface name lagg0.25 type vlan vrf 25 mtu 1500 address fcff:25::10:1:0:1/64 group vlan
set interface name lagg0.28 type vlan vrf 28 mtu 1500 address 203.0.113.89/24 group vlan vid 28 parent lagg0
set interface name lagg0.28 type vlan vrf 28 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan
set interface name vlan48 type vlan vrf 48 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 48 parent lagg0
set interface name vlan27.1 type vlan vrf 27 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 1 parent vlan27
set interface name lagg0.180 type vlan vrf 180 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 180 parent lagg0
set interface name lagg0.20 type vlan vrf 20 mtu 1500 address 10.1.64.1/18 group vlan vid 20 parent lagg0
set interface name lagg0.20 type vlan vrf 20 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan
set interface name lagg0.20 type vlan vrf 20 mtu 1500 address fcff:20::10:1:64:1/64 group vlan
set interface name vlan48.5 type vlan vrf 48 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 5 parent vlan48
set interface name lagg0.255 type vlan vrf 0 mtu 1500 group vlan vid 255 parent lagg0
set interface name vlan180.4 type vlan vrf 180 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 4 parent vlan180
set interface name vlan180.4 type vlan vrf 180 mtu 1496 address 2001:470:e845::443/128 group vlan
set interface name vlan180 type vlan vrf 0 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 0 parent 
set interface name lagg0.18 type vlan vrf 18 mtu 1500 address 192.168.64.129/25 group vlan vid 18 parent lagg0
set interface name lagg0.18 type vlan vrf 18 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan
set interface name lagg0.18 type vlan vrf 18 mtu 1500 address fcff:18::192:168:64:129/112 group vlan
set interface name lagg0.27 type vlan vrf 27 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 27 parent lagg0
set interface name vlan48.1 type vlan vrf 48 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 1 parent vlan48
set interface name vlan53 type vlan vrf 53 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 53 parent lagg0
set interface name vlan53.3 type vlan vrf 53 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 3 parent vlan53
set interface name vlan53.3 type vlan vrf 53 mtu 1496 address 2001:470:e845::53/128 group vlan
set interface name vlan81.1 type vlan vrf 81 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 1 parent vlan81
set interface name vlan81.1 type vlan vrf 81 mtu 1496 address 192.88.99.0/32 group vlan
set interface name vlan80.2 type vlan vrf 80 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 2 parent vlan80
set interface name vlan80.2 type vlan vrf 80 mtu 1496 address fcff:80::192:88:99:2/128 group vlan
set interface name lagg0.16 type vlan vrf 16 mtu 1500 address 192.168.32.129/25 group vlan vid 16 parent lagg0
set interface name lagg0.16 type vlan vrf 16 mtu 1500 address fe80::2e0:4cff:fe17:5512/64 group vlan
set interface name lagg0.16 type vlan vrf 16 mtu 1500 address fcff:16::192:168:32:129/64 group vlan
set interface name vlan53.1 type vlan vrf 53 mtu 1496 address fe80::2e0:4cff:fe17:5512/64 group vlan vid 1 parent vlan53
set interface name vlan53.1 type vlan vrf 53 mtu 1496 address fcff::53/128 group vlan
set interface name gif1 vrf 48 mtu 1480 address 2001:470:a:56f::2/128 group gif source 192.168.1.156 destination 216.218.226.238
set interface name gif1 vrf 48 mtu 1480 address fe80::2e0:4cff:fe17:5512/64 group gif
set route protocol static dest 0.0.0.0/8 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 0.0.0.0/0 nexthop 192.0.0.31 interface epair10a
set route protocol static dest 10.1.0.0/18 nexthop 192.0.0.15 interface epair2a
set route protocol static dest 10.1.64.0/18 nexthop 192.0.0.35 interface epair12a
set route protocol static dest 10.255.255.0/24 nexthop 192.0.0.33 interface epair11a
set route protocol static dest 100.64.0.0/10 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 127.0.0.0/8 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 169.254.0.0/16 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 172.16.0.0/12 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 192.0.0.0/24 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 192.0.0.10/32 nexthop link#4 interface lo0
set route protocol static dest 192.0.0.12/32 nexthop link#6 interface lo0
set route protocol static dest 192.0.0.14/32 nexthop link#14 interface lo0
set route protocol static dest 192.0.0.16/32 nexthop link#16 interface lo0
set route protocol static dest 192.0.0.18/32 nexthop link#18 interface lo0
set route protocol static dest 192.0.0.20/32 nexthop link#20 interface lo0
set route protocol static dest 192.0.0.22/32 nexthop link#22 interface lo0
set route protocol static dest 192.0.0.24/32 nexthop link#24 interface lo0
set route protocol static dest 192.0.0.28/32 nexthop link#26 interface lo0
set route protocol static dest 192.0.0.30/32 nexthop link#8 interface lo0
set route protocol static dest 192.0.0.32/32 nexthop link#10 interface lo0
set route protocol static dest 192.0.0.34/32 nexthop link#12 interface lo0
set route protocol static dest 192.0.0.36/32 nexthop link#62 interface lo0
set route protocol static dest 192.0.0.38/32 nexthop link#73 interface lo0
set route protocol static dest 192.0.0.40/32 nexthop link#75 interface lo0
set route protocol static dest 192.0.0.42/32 nexthop link#86 interface lo0
set route protocol static dest 192.0.0.44/32 nexthop link#88 interface lo0
set route protocol static dest 192.0.2.2/32 nexthop 192.0.0.41 interface epair22a
set route protocol static dest 192.88.99.0/32 nexthop 192.0.0.23 interface epair6a
set route protocol static dest 192.88.99.0/24 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 192.88.99.2/32 nexthop 192.0.0.21 interface epair5a
set route protocol static dest 192.168.0.0/16 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 192.168.32.128/25 nexthop 192.0.0.11 interface epair0a
set route protocol static dest 192.168.64.128/25 nexthop 192.0.0.13 interface epair1a
set route protocol static dest 192.168.72.128/25 nexthop 192.0.0.17 interface epair3a
set route protocol static dest 198.18.0.0/15 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 198.51.100.0/32 nexthop 192.0.0.37 interface epair19a
set route protocol static dest 198.51.100.0/24 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 203.0.113.0/24 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 203.0.113.53/32 nexthop 192.0.0.19 interface epair4a
set route protocol static dest 203.0.113.89/32 nexthop 192.0.0.39 interface epair21a
set route protocol static dest 203.0.113.123/32 nexthop 192.0.0.25 interface epair7a
set route protocol static dest 223.252.0.0/24 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest 240.0.0.0/4 nexthop 127.0.0.1 interface lo0 reject
set route protocol static dest ::/96 nexthop ::1 interface lo0 reject
set route protocol static dest ::1/128 nexthop link#3 interface lo0
set route protocol static dest ::ffff:0.0.0.0/96 nexthop ::1 interface lo0 reject
set route protocol static dest ::ffff:0:0:0/96 nexthop ::1 interface lo0 reject
set route protocol static dest 64:ff9b::/96 nexthop ::1 interface lo0 reject
set route protocol static dest 64:ff9b:1::/48 nexthop ::1 interface lo0 reject
set route protocol static dest 100::/64 nexthop ::1 interface lo0 reject
set route protocol static dest 2000::/3 nexthop fcfc:48::192:0:0:29 interface epair9a
set route protocol static dest 2001::/32 nexthop ::1 interface lo0 reject
set route protocol static dest 2001:20::/28 nexthop ::1 interface lo0 reject
set route protocol static dest 2001:470:e845::23/128 nexthop fcfc:180::192:0:0:37 interface epair19a
set route protocol static dest 2001:470:e845::53/128 nexthop fcfc:53::192:0:0:19 interface epair4a
set route protocol static dest 2001:470:e845::80/128 nexthop fcfc:180::192:0:0:37 interface epair19a
set route protocol static dest 2001:470:e845::443/128 nexthop fcfc:180::192:0:0:37 interface epair19a
set route protocol static dest 2001:470:e845::3128/128 nexthop fcfc:80::192:0:0:21 interface epair5a
set route protocol static dest 2001:470:e845::80:443/128 nexthop fcfc:180::192:0:0:37 interface epair19a
set route protocol static dest 2001:470:e845::555:1212/128 nexthop fcfc:30::192:0:0:17 interface epair3a
set route protocol static dest 2001:470:e845:f::/64 nexthop fcfc:30::192:0:0:17 interface epair3a
set route protocol static dest 2001:db8::/32 nexthop ::1 interface lo0 reject
set route protocol static dest 2002::/16 nexthop ::1 interface lo0 reject
set route protocol static dest 3fff::/20 nexthop ::1 interface lo0 reject
set route protocol static dest 5f00::/16 nexthop ::1 interface lo0 reject
set route protocol static dest fc00::/7 nexthop ::1 interface lo0 reject
set route protocol static dest fcfc:1::192:0:0:32/128 nexthop link#10 interface lo0
set route protocol static dest fcfc:16::192:0:0:10/128 nexthop link#4 interface lo0
set route protocol static dest fcfc:18::192:0:0:12/128 nexthop link#6 interface lo0
set route protocol static dest fcfc:20::192:0:0:34/128 nexthop link#12 interface lo0
set route protocol static dest fcfc:25::192:0:0:14/128 nexthop link#14 interface lo0
set route protocol static dest fcfc:30::192:0:0:16/128 nexthop link#16 interface lo0
set route protocol static dest fcfc:48::192:0:0:28/128 nexthop link#26 interface lo0
set route protocol static dest fcfc:53::192:0:0:18/128 nexthop link#18 interface lo0
set route protocol static dest fcfc:80::192:0:0:20/128 nexthop link#20 interface lo0
set route protocol static dest fcfc:81::192:0:0:22/128 nexthop link#22 interface lo0
set route protocol static dest fcfc:123::192:0:0:24/128 nexthop link#24 interface lo0
set route protocol static dest fcfc:180::192:0:0:36/128 nexthop link#62 interface lo0
set route protocol static dest fcfc:180::192:0:0:38/128 nexthop link#73 interface lo0
set route protocol static dest fcfc:180::192:0:0:40/128 nexthop link#75 interface lo0
set route protocol static dest fcfc:242::192:0:0:42/128 nexthop link#86 interface lo0
set route protocol static dest fcfc:243::192:0:0:44/128 nexthop link#88 interface lo0
set route protocol static dest fcfc:255::192:0:0:30/128 nexthop link#8 interface lo0
set route protocol static dest fcff::53/128 nexthop fcfc:53::192:0:0:19 interface epair4a
set route protocol static dest fcff::123/128 nexthop fcfc:123::192:0:0:25 interface epair7a
set route protocol static dest fcff::3128/128 nexthop fcfc:81::192:0:0:23 interface epair6a
set route protocol static dest fcff:16::/32 nexthop fcfc:16::192:0:0:11 interface epair0a
set route protocol static dest fcff:18::/32 nexthop fcfc:18::192:0:0:13 interface epair1a
set route protocol static dest fcff:20::/32 nexthop fcfc:20::192:0:0:35 interface epair12a
set route protocol static dest fcff:25::/32 nexthop fcfc:25::192:0:0:15 interface epair2a
set route protocol static dest fcff:30::/32 nexthop fcfc:30::192:0:0:17 interface epair3a
set route protocol static dest fcff:48::192:88:99:255/128 nexthop fcfc:48::192:0:0:29 interface epair9a
set route protocol static dest fcff:80::192:88:99:2/128 nexthop fcfc:80::192:0:0:21 interface epair5a
set route protocol static dest fcff:180::198:51:100:0/128 nexthop fcfc:180::192:0:0:37 interface epair19a
set route protocol static dest fdb8:3260:d80f:a056::/64 nexthop fcfc:255::192:0:0:31 interface epair10a
set route protocol static dest fe80::/10 nexthop ::1 interface lo0 reject
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#1 interface lo0 scope re0
set route protocol static dest fe80::2e0:4cff:fe17:5513/128 nexthop link#2 interface lo0 scope re1
set route protocol static dest fe80::1/128 nexthop link#3 interface lo0 scope lo0
set route protocol static dest fe80::6a:fff:fe48:5c0a/128 nexthop link#4 interface lo0 scope epair0a
set route protocol static dest fe80::97:74ff:fe8c:e60a/128 nexthop link#6 interface lo0 scope epair1a
set route protocol static dest fe80::b7:95ff:fe19:90a/128 nexthop link#8 interface lo0 scope epair10a
set route protocol static dest fe80::53:2ff:feab:c00a/128 nexthop link#10 interface lo0 scope epair11a
set route protocol static dest fe80::a3:46ff:fef9:4d0a/128 nexthop link#12 interface lo0 scope epair12a
set route protocol static dest fe80::e3:5dff:fe5e:210a/128 nexthop link#14 interface lo0 scope epair2a
set route protocol static dest fe80::fe:bff:fe4e:150a/128 nexthop link#16 interface lo0 scope epair3a
set route protocol static dest fe80::3a:cbff:feba:280a/128 nexthop link#18 interface lo0 scope epair4a
set route protocol static dest fe80::8f:13ff:fe1c:d0a/128 nexthop link#20 interface lo0 scope epair5a
set route protocol static dest fe80::63:c8ff:fe2c:e70a/128 nexthop link#22 interface lo0 scope epair6a
set route protocol static dest fe80::82:b9ff:fe75:cc0a/128 nexthop link#24 interface lo0 scope epair7a
set route protocol static dest fe80::6d:85ff:fe3c:450a/128 nexthop link#26 interface lo0 scope epair9a
set route protocol static dest fe80::1/128 nexthop link#29 interface lo0 scope lo1
set route protocol static dest fe80::1/128 nexthop link#30 interface lo0 scope lo20
set route protocol static dest fe80::1/128 nexthop link#32 interface lo0 scope lo25
set route protocol static dest fe80::37:58ff:fec9:ed0a/128 nexthop link#38 interface lo0 scope epair13a
set route protocol static dest fe80::37:58ff:fec9:ed0b/128 nexthop link#39 interface lo0 scope epair13b
set route protocol static dest fe80::b3:16ff:fe82:ce0a/128 nexthop link#40 interface lo0 scope epair14a
set route protocol static dest fe80::b3:16ff:fe82:ce0b/128 nexthop link#41 interface lo0 scope epair14b
set route protocol static dest fe80::57:35ff:feda:b20a/128 nexthop link#42 interface lo0 scope epair15a
set route protocol static dest fe80::57:35ff:feda:b20b/128 nexthop link#43 interface lo0 scope epair15b
set route protocol static dest fe80::55:43ff:fe8f:a10a/128 nexthop link#48 interface lo0 scope epair18a
set route protocol static dest fe80::55:43ff:fe8f:a10b/128 nexthop link#49 interface lo0 scope epair18b
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#50 interface lo0 scope vlan48
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#51 interface lo0 scope vlan53
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#52 interface lo0 scope vlan80
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#53 interface lo0 scope vlan81
set route protocol static dest fe80::1/128 nexthop link#55 interface lo0 scope lo255
set route protocol static dest fe80::1/128 nexthop link#56 interface lo0 scope lo48
set route protocol static dest fe80::1/128 nexthop link#57 interface lo0 scope lo53
set route protocol static dest fe80::1/128 nexthop link#58 interface lo0 scope lo81
set route protocol static dest fe80::1/128 nexthop link#59 interface lo0 scope lo80
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#60 interface lo0 scope vlan180
set route protocol static dest fe80::1/128 nexthop link#61 interface lo0 scope lo180
set route protocol static dest fe80::87:1ff:fe11:580a/128 nexthop link#62 interface lo0 scope epair19a
set route protocol static dest fe80::30:2fff:fe76:950a/128 nexthop link#64 interface lo0 scope epair20a
set route protocol static dest fe80::30:2fff:fe76:950b/128 nexthop link#65 interface lo0 scope epair20b
set route protocol static dest fe80::1/128 nexthop link#67 interface lo0 scope lo16
set route protocol static dest fe80::1/128 nexthop link#68 interface lo0 scope lo26
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#69 interface lo0 scope vlan26
set route protocol static dest fe80::1/128 nexthop link#70 interface lo0 scope lo27
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#71 interface lo0 scope vlan27
set route protocol static dest fe80::8a:cdff:fe38:9f0a/128 nexthop link#73 interface lo0 scope epair21a
set route protocol static dest fe80::f3:98ff:fe1a:590a/128 nexthop link#75 interface lo0 scope epair22a
set route protocol static dest fe80::1/128 nexthop link#78 interface lo0 scope lo128
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#79 interface lo0 scope vlan128
set route protocol static dest fe80::5a9c:fcff:fe10:fff5/128 nexthop link#84 interface lo0 scope tap0
set route protocol static dest fe80::5a9c:fcff:fe10:ff9e/128 nexthop link#85 interface lo0 scope tap1
set route protocol static dest fe80::67:4fff:fee1:5e0a/128 nexthop link#86 interface lo0 scope epair23a
set route protocol static dest fe80::d4:41ff:fe08:fa0a/128 nexthop link#88 interface lo0 scope epair24a
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#95 interface lo0 scope lagg0.26
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#96 interface lo0 scope lagg0.27
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#100 interface lo0 scope lagg0.180
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#102 interface lo0 scope vlan48.1
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#103 interface lo0 scope vlan48.2
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#104 interface lo0 scope vlan48.3
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#105 interface lo0 scope vlan48.4
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#106 interface lo0 scope vlan48.5
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#107 interface lo0 scope vlan53.1
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#108 interface lo0 scope vlan53.2
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#109 interface lo0 scope vlan53.3
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#110 interface lo0 scope vlan53.4
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#111 interface lo0 scope vlan80.1
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#112 interface lo0 scope vlan80.2
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#113 interface lo0 scope vlan80.3
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#114 interface lo0 scope vlan80.4
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#115 interface lo0 scope vlan81.1
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#116 interface lo0 scope vlan81.2
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#117 interface lo0 scope vlan81.3
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#118 interface lo0 scope vlan180.1
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#119 interface lo0 scope vlan180.2
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#120 interface lo0 scope vlan180.3
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#121 interface lo0 scope vlan180.4
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#122 interface lo0 scope vlan180.5
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#123 interface lo0 scope vlan180.6
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#124 interface lo0 scope vlan180.7
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#125 interface lo0 scope vlan26.1
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#128 interface lo0 scope vlan27.1
set route protocol static dest fe80::2e0:4cff:fe17:5512/128 nexthop link#129 interface lo0 scope vlan128.1
set route protocol static dest ff02::/16 nexthop ::1 interface lo0 reject