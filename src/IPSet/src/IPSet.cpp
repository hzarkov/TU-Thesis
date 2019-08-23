#include "IPSet.hpp"
#include "System.hpp"

#include <stdexcept>

IPSet::IPSet(std::string name, std::string type)
:name(name)
{
    std::string cmd = "ipset create " + name + " " + type + " family inet";
    if(0 != System::call(cmd)) //TODO: remove hard-coded information like 'family' and 'inet's
    {
        throw std::runtime_error("Failed to create ipset with name '" + name + "' and '" + type + "' type: " + cmd);
    }
}

IPSet::~IPSet()
{
    System::call("ipset destroy " + this->name);
}