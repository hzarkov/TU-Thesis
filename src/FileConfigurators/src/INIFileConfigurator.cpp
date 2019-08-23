#include "INIFileConfigurator.hpp"
#include "EthernetController.hpp"
#include "Logger.hpp"
#include "INIReader.h"

#include <algorithm>
#include <sstream>

constexpr uint DEFAULT_DHCP_LEASE=7200;

INIFileConfigurator::INIFileConfigurator(std::shared_ptr<NetworkManager> nm)
:FileConfigurator(nm)
{

}
void INIFileConfigurator::config(std::map<std::string, std::string> conf)
{
}

void INIFileConfigurator::exec()
{
    INIReader reader(this->file_name);

    if (reader.ParseError() < 0) {
        throw std::runtime_error("Can't load " + this->file_name);
    }
    std::set<std::string> sections = reader.Sections();
    for (std::set<std::string>::iterator it = sections.begin(); it != sections.end(); ++it)
    {
        std::string interface_name = *it;
        std::string interface_type(reader.Get(interface_name, "type", ""));
        std::transform(interface_type.begin(), interface_type.end(), interface_type.begin(), ::tolower);
        if(interface_type.empty())
        {
            ErrorLogger << interface_name << " is without type, skipping this configuration!" << std::endl;
            continue;
        }
        else if("ethernet" == interface_type)
        {
            std::shared_ptr<EthernetController> econtroller;
            try
            { 
                econtroller = std::dynamic_pointer_cast<EthernetController>(
                        this->network_manager->getInterface(interface_name));
            }
            catch(std::out_of_range& e)
            {
                WarningLogger << "Failed to get " << interface_name << ": " << e.what() << std::endl;
                continue;
            }

            std::string ip(reader.Get(interface_name, "ip", ""));
            if(!ip.empty()) // static if IP is set
            {
                std::string netmask(reader.Get(interface_name, "netmask", "255.255.255.0"));
                std::string gw(reader.Get(interface_name, "gateway", ""));
                std::string dns1(reader.Get(interface_name, "dns_server1", ""));
                std::string dns2(reader.Get(interface_name, "dns_server2", ""));
                std::vector<std::string> dns_servers;
                if(!dns1.empty())
                {
                    dns_servers.push_back(dns1);
                    if(!dns2.empty())
                    {
                        dns_servers.push_back(dns2);
                    }
                }
               
                econtroller->setStatic(ip, netmask, gw, dns_servers);
                std::string dhcp_range(reader.Get(interface_name, "dhcp-range", ""));
                if(!dhcp_range.empty())
                {
                    std::stringstream ss(dhcp_range);
                    std::string starting_ip, ending_ip;
                    std::getline(ss, starting_ip, ',');
                    std::getline(ss, ending_ip, ',');
                    uint lease = reader.GetInteger(interface_name, "lease", DEFAULT_DHCP_LEASE);
                    econtroller->setDHCPServer(starting_ip, ending_ip, lease);
                }
            }
            else // dhcp if IP is not set
            {
                econtroller->useDHCP();
            }
        }
        else
        {
            ErrorLogger << "Unknown " << interface_type << " type for " << interface_name << std::endl;
        }
    }    
}

extern "C"
{
    Plugin* allocator(std::shared_ptr<NetworkManager> nm)
    {
        return new INIFileConfigurator(nm);
    }
}