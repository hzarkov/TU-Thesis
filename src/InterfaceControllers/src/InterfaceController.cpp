#include "InterfaceController.hpp"

InterfaceController::InterfaceController(std::string interface_name)
:interface(std::make_unique<Interface>(interface_name))
{

}

std::string InterfaceController::getGW()
{
    return this->interface->getGW().toString();
}