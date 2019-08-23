#include "RouteRule.hpp"
#include "System.hpp"

RouteRule::RouteRule(std::string rule_data)
:rule_data(rule_data)
{
    System:call("ip rule add " + rule_data);
}

RouteRule::~RouteRule()
{
    System:call("ip rule del " + rule_data);
}