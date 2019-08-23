#include "RouteRule.hpp"
#include "System.hpp"

#include <stdexcept>

RouteRule::RouteRule(uint priority, uint lookup, std::string additional_data)
:priority(priority), lookup(lookup), additional_data(additional_data)
{
    std::string cmd = "ip rule add prio " + std::to_string(priority) + " lookup " + std::to_string(lookup) + " " + additional_data;
    if( 0 != System::call(cmd))
    {
        throw std::runtime_error("Failed to add routing rule: " + cmd);
    }
}

uint RouteRule::getPriority()
{
    return this->priority;
}

uint RouteRule::getLookup()
{
    return this->lookup;
}

RouteRule::~RouteRule()
{
    System::call("ip rule del prio " + std::to_string(priority) + " lookup " + std::to_string(lookup) + " " + additional_data);
}