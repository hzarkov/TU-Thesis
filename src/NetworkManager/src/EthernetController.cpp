#include "EthernetController.hpp"
#include "System.hpp"


void EthernetController::configureInterface(std::string ip, std::string netmask, std::string gateway, std::vector<std::string> dns_servers)
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

EthernetController::EthernetController(std::string interface_name)
:interface(std::make_unique<Interface>(interface_name)),dhcp_client(interface_name)
{
    dhcp_client.registerConfigCallback(std::bind(&EthernetController::configureInterface, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void EthernetController::setStatic(std::string ip, std::string netmask, std::string gateway, std::vector<std::string> dns_servers)
{
    dhcp_client.stop();
    this->configureInterface(ip, netmask, gateway, dns_servers);
}

void EthernetController::useDHCP()
{
    dhcp_client.start();
}