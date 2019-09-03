#include "EthernetController.hpp"
#include "System.hpp"
#include "Logger.hpp"

void EthernetController::configureInterface(std::string ip, std::string netmask, std::string gateway, std::vector<std::string> dns_servers)
{
    DebugLogger << "Hello: " << std::endl;
    this->interface->setIP(IP(ip), IP(netmask));
    DebugLogger << "IP: " << ip << std::endl;
    DebugLogger << "Netmask: " << netmask << std::endl;
    this->interface->setGW(IP(gateway));
    DebugLogger << "Gateway: " << gateway << std::endl;
    std::vector<IP> ip_dns_servers;
    for(std::string server : dns_servers)
    {
        ip_dns_servers.push_back(IP(server));
        DebugLogger << "DNS server: " << gateway << std::endl;
    }
    this->interface->setDNS(ip_dns_servers);
}

EthernetController::EthernetController(std::string interface_name, std::shared_ptr<DHCPServer> dhcp_server)
:InterfaceController(interface_name),dhcp_server(dhcp_server), dhcp_client(interface_name)
{
    DebugLogger << "Hello: " << std::endl;
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
    DebugLogger << "Hello: " << std::endl;
    dhcp_client.start();
}

void EthernetController::setDHCPServer(std::string ip_range_start, std::string ip_range_end, uint lease )
{
    this->dhcp_server_configuration = this->dhcp_server->createConfiguration(this->interface->getName(), ip_range_start, ip_range_end, lease);
}