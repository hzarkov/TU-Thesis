#include "RouteRule.hpp"
#include "System.hpp"

#include <stdexcept>

RouteRule::RouteRule(std::string rule_data)
:rule_data(rule_data)
{
    std::string cmd = "ip rule add " + rule_data;
    if( 0 != System::call(cmd))
    {
        throw std::runtime_error("Failed to add routing rule: " + cmd);
    }
}

RouteRule::~RouteRule()
{
    System::call("ip rule del " + rule_data);
}