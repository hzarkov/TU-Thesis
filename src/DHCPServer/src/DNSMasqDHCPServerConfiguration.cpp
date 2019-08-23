#include "DNSMasqDHCPServerConfiguration.hpp"

#include <fstream>
#include <functional>

const std::string base_dhcp_file_name = "dhcp-";

DNSMasqDHCPServerConfiguration::DNSMasqDHCPServerConfiguration(std::shared_ptr<DNSMasqController> dnsmasq_controller, std::string interface_name, std::string ip_range_start, std::string ip_range_end, uint lease)
:dnsmasq_controller(dnsmasq_controller)
{
    if(ip_range_start.empty())
    {
        throw std::invalid_argument("starting ip is empty!");
    }
    if(ip_range_end.empty())
    {
        throw std::invalid_argument("ending ip is empty!");
    }
    std::string lease_string;
    if(lease < 120) 
    {
        throw std::invalid_argument("(lease time cannot be less then 2 minutes(120s)");
    }
    else if(lease > 3600)
    {
        lease_string = std::string(",") + std::to_string((lease/60)/60) + "h";
    }
    else
    {
        lease_string = std::string(",") + std::to_string(lease/60) + "m";
    }
    this->conf = this->dnsmasq_controller->addConfiguration(base_dhcp_file_name + ip_range_start);
    conf->addConfiguration("interface", interface_name);
    conf->addConfiguration("dhcp-range", interface_name + "," + ip_range_start + "," + ip_range_end + lease_string);
    this->dnsmasq_controller->reloadConfiguration();
}
