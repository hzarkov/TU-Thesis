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
        void routeConfiguration(std::string option);
    public:
        Route(IP destination, IP gateway=IP("0.0.0.0"), std::string interface_name="", int metric=0);
        ~Route();

        IP getDestination();
        IP getGateway();
        int getMetric();
        std::string getInterfaceName();
};
#endif