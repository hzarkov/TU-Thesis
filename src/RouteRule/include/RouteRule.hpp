#ifndef ROUTE_RULE_HPP
#define ROUTE_RULE_HPP
#include <string>

class RouteRule
{
public:
    RouteRule(uint priority, uint lookup, std::string additional_data = "");
    uint getPriority();
    uint getLookup();
    ~RouteRule();
private:
    uint priority;
    uint lookup;
    std::string additional_data;
};
#endif