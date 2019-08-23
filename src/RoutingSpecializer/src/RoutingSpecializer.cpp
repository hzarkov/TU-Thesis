#include "RoutingSpecializer.hpp"
#include "Logger.hpp"

RoutingSpecializer::RoutingSpecializer(std::shared_ptr<NetworkManager> nm)
:Plugin(nm)
{
   
}

void RoutingSpecializer::configure(std::map<std::string, std::string> conf)
{
    for(auto configuration : conf)
    {
        this->traffic_specialization = std::make_unique<TrafficSpecialization>(this->network_manager, configuration.first);
        std::stringstream ss(configuration.second);
        std::string route;
        while (std::getline(ss, route, ',')) 
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
                 WarningLogger << "Unknown domain type of '" + route << "'" << std::endl;
            } 
        }
    }   
}

void RoutingSpecializer::exec()
{

}

extern "C"
{
    Plugin* allocator(std::shared_ptr<NetworkManager> nm)
    {
        return new RoutingSpecializer(nm);
    }
}