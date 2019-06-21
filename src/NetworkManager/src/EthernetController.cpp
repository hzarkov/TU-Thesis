#include "EthernetController.hpp"
#include "System.hpp"

EthernetController::EthernetController(std::string interface_name)
:interface(std::make_unique<Interface>(interface_name))
{

}

void EthernetController::setStatic(std::string ip, std::string netmask, std::string gateway, std::vector<std::string> dns_servers)
{
    this->interface->setIP(IP(ip), IP(netmask));
    this->interface->setGW(IP(gateway));
    std::vector<IP> ip_dns_servers;
    for(std::string server : dns_servers)
    {
        ip_dns_servers.push_back(IP(server));
    }
    this->interface->setDNS(ip_dns_servers);
}

/*void EthernetController::useDHCP()
{
    System::call("dhclient "+ this->interface->getName());
}*/