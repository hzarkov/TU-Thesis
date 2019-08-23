#include "RoutingSpecializer.hpp"
#include "Logger.hpp"

RoutingSpecializer::RoutingSpecializer(std::shared_ptr<NetworkManager> nm, std::string interface, std::vector<std::string> route_domains)
:TrafficSpecializer(nm)
{
    this->traffic_specialization = std::make_unique<TrafficSpecialization>(nm, interface);
    for(auto route : route_domains)
    {
        if(1) //ToDo: Check if route is IP or domain
        {
            try
            {
                this->traffic_specialization->addIP(route);
            }
            catch(std::exception& e)
            {
                ErrorLogger << e.what() << std::endl;
            }
        }
        else if(1)
        {
            this->traffic_specialization->addDomain(route);
        }
        else
        {
            throw std::invalid_argument("Unknown domain type: " + route);
        } 
    }
}

extern "C"
{
    TrafficSpecializer* allocator(std::shared_ptr<NetworkManager> nm, std::string interface, std::vector<std::string> route_domains)
    {
        return new RoutingSpecializer(nm, interface, route_domains);
    }
    void deallocator(TrafficSpecializer* p)
    {
        delete p;
    }
}