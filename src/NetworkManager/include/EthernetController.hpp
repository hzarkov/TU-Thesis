#ifndef ETHERNET_CONTROLLER_HPP
#define ETHERNET_CONTROLLER_HPP
#include "InterfaceController.hpp"
#include "DHCPClient.hpp"
#include "DHCPServer.hpp"

#include <string>
#include <memory>

class EthernetController : public InterfaceController
{
private:
    std::shared_ptr<DHCPServer> dhcp_server;
    std::shared_ptr<DHCPServerConfiguration> dhcp_server_configuration;
    DHCPClient dhcp_client;
    void configureInterface(std::string ip, std::string netmask, std::string gateway, std::vector<std::string> dns_servers);
public:
    EthernetController(std::string interface_name, std::shared_ptr<DHCPServer> dhcp_server);
    void setStatic(std::string IP, std::string netmask="255.255.255.255", std::string gateway="", std::vector<std::string> dns_servers=std::vector<std::string>());
    void useDHCP();
    void setDHCPServer(std::string ip_range_start,std::string ip_range_end, uint lease=7200);
};
#endif