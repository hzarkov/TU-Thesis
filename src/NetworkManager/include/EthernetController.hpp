#ifndef ETHERNET_CONTROLLER_HPP
#define ETHERNET_CONTROLLER_HPP
#include "InterfaceController.hpp"
#include "Interface.hpp"

#include <string>
#include <memory>

class EthernetController : public InterfaceController
{
private:
    std::unique_ptr<Interface> interface;
public:
    EthernetController(std::string interface_name);
    void setStatic(std::string IP, std::string netmask="255.255.255.255", std::string gateway="", std::vector<std::string> dns_servers=std::vector<std::string>());
    /*void useDHCP();
    void setDHCPServer(std::string ip_range_start,std::string ip_range_end, int lease );*/
};
#endif