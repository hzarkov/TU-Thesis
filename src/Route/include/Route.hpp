#ifndef ROUTE_HPP
#define ROUTE_HPP
#include "IP.hpp"
#include <string>

class Route
{
private:
    IP destination;
    IP gateway;
    int metric;
    std::string interface_name;
    std::string table_name;
    void routeConfiguration(std::string option);
public:
    Route(IP destination, IP gateway=IP("0.0.0.0"), std::string interface_name="", int metric=0, std::string table_name="");
    ~Route();

    IP getDestination();
    IP getGateway();
    std::string getTable();
    int getMetric();
    std::string getInterfaceName();
    bool operator==(const Route& route);
};
#endif