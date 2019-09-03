#include "InterfaceController.hpp"

InterfaceController::InterfaceController(std::string interface_name)
:interface(std::make_shared<Interface>(interface_name))
{

}

std::string InterfaceController::getGW()
{
    return this->interface->getGW().toString();
}

std::string InterfaceController::getName()
{
    return this->interface->getName();
}

std::vector<std::string> InterfaceController::getDNSServers()
{
    std::vector<std::string> result;
    for(auto dns_server: this->interface->getDNSServers())
    {
        result.push_back(dns_server.toString());
    }
    return result;
}