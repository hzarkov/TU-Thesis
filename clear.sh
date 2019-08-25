iptables -t mangle -D PREROUTING -m set --match-set enx503eaa8b4c8c_ipset dst -m mark --mark 0x0 -j MARK --set-xmark 0x1/0xffffffff
iptables -t mangle -D OUTPUT -m set --match-set enx503eaa8b4c8c_ipset dst -m mark --mark 0x0 -j MARK --set-xmark 0x1/0xffffffff
ip route del 0.0.0.0/0 via 192.168.157.1 dev enx503eaa8b4c8c metric 0 table 101
ip rule del prio 101 lookup 101 fwmark 1
ipset destroy enx503eaa8b4c8c_ipset
