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

std::string IPSet::getName()
{
    return this->name;
}


void IPSet::addIP(std::string ip)
{
    std::string cmd = "ipset add " + this->name + " " + ip;
    if (0 != System::call(cmd))
    {
        throw std::runtime_error("Failed to add '" + ip + "' to '" + this->name + "' ipset using: " + cmd);
    }
    ips.push_back(ip);
}

std::vector<std::string> IPSet::getIPs()
{
    return this->ips;
}

IPSet::~IPSet()
{
    System::call("ipset destroy " + this->name);
}