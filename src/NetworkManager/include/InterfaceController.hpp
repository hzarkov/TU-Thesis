#ifndef INTERFACE_CONTROLLER_HPP
#define INTERFACE_CONTROLLER_HPP
#include "Interface.hpp"
#include <memory>

class InterfaceController
{
public:
    InterfaceController(std::string interface_name);
    virtual ~InterfaceController() = default;
    std::string getGW();
protected:
    std::unique_ptr<Interface> interface;
};
#endif