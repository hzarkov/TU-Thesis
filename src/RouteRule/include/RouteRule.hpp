#ifndef ROUTE_RULE_HPP
#define ROUTE_RULE_HPP
#include <string>

class RouteRule
{
public:
    RouteRule(std::string rule_data);
    ~RouteRule();
private:
   std::string rule_data;
};
#endif