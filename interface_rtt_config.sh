#!/bin/sh

#sudo vppctl "exec /tmp/route_add_r20k_ipv6_with_prefix.txt"
sudo vppctl "set int ip address FortyGigabitEthernet4/0/0 9000::2/48"
sudo vppctl "set int ip address FortyGigabitEthernet2/0/0 4000::2/48"
sudo vppctl "set int ip address FortyGigabitEthernet4/0/0 20.1.1.2/24"
sudo vppctl "set int ip address FortyGigabitEthernet2/0/0 10.1.1.2/24"
sudo vppctl "set interface state FortyGigabitEthernet2/0/0 up"
sudo vppctl "set interface state FortyGigabitEthernet4/0/0 up"
sudo vppctl "set ip6 neighbor FortyGigabitEthernet4/0/0 9000::1 3c:fd:fe:a6:8e:f8"
sudo vppctl "ip route add 0.0.0.0/0 via 20.1.1.1"
sudo vppctl "set ip6 neighbor FortyGigabitEthernet2/0/0 4000::1 3c:fd:fe:a6:92:b0"
sudo vppctl "ip route add 4000::1/32 via 4000::1"
