#include "IPSet.hpp"
#include "System.hpp"

#include <stdexcept>
#include <algorithm>

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
    if(this->ips.end() == std::find(this->ips.begin(), this->ips.end(), ip))
    {
        std::string cmd = "ipset add " + this->name + " " + ip;
        if (0 != System::call(cmd))
        {
            throw std::runtime_error("Failed to add '" + ip + "' to '" + this->name + "' ipset using: " + cmd);
        }
        this->ips.push_back(ip);
    }
}

void IPSet::clearIPs()
{
    std::string cmd = "ipset flush " + this->name;
    if( 0 != System::call(cmd))
    {
        throw std::runtime_error("Failed to clear " + this->name);
    }
    this->ips.clear();
}

std::vector<std::string> IPSet::getIPs()
{
    return this->ips;
}

IPSet::~IPSet()
{
    System::call("ipset destroy " + this->name);
}