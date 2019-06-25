#include "DNSMasqDHCPServerConfiguration.hpp"

#include <fstream>
#include <functional>

const std::string dhcp_configuration_path = "/etc/dnsmasq.d/";
const std::string base_dhcp_file_name = "dhcp-";
const std::string dhcp_file_extension = ".conf";

DNSMasqDHCPServerConfiguration::DNSMasqDHCPServerConfiguration(std::shared_ptr<std::mutex> conf_mutex, std::shared_ptr<DNSMasqController> dnsmasq_controller, std::string interface_name, std::string ip_range_start, std::string ip_range_end, uint lease)
:conf_mutex(conf_mutex),dnsmasq_controller(dnsmasq_controller)
{
    std::lock_guard<std::mutex> conf_mutex_lock_guard(*this->conf_mutex);
    std::hash<std::string> hasher;
    size_t file_identity = hasher(ip_range_start);

    std::ofstream conf_file(dhcp_configuration_path + base_dhcp_file_name + std::to_string(file_identity) + dhcp_file_extension);
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
    conf_file << "interface=" << interface_name << std::endl;
    conf_file << "dhcp-range=" << interface_name << "," << ip_range_start << "," << ip_range_end << lease_string << std::endl;
    conf_file.close();
    this->dnsmasq_controller->reloadConfiguration();
}

DNSMasqDHCPServerConfiguration::~DNSMasqDHCPServerConfiguration()
{
    std::lock_guard<std::mutex> conf_mutex_lock_guard(*this->conf_mutex);
    
}