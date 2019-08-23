#ifndef DHCP_SERVER_HPP
#define DHCP_SERVER_HPP

#include "DNSMasqDHCPServerConfiguration.hpp"
#include "DNSMasqController.hpp"

#include <mutex>
#include <string>
#include <memory>

class DHCPServer
{
    std::shared_ptr<DNSMasqController> dnsmasq_controller;
public:
    DHCPServer(std::shared_ptr<DNSMasqController> dnsmasq_controller);
    ~DHCPServer() = default;
    std::shared_ptr<DHCPServerConfiguration> createConfiguration(std::string interface_name, std::string ip_range_start, std::string ip_range_end, uint lease=7200);
};
#endif