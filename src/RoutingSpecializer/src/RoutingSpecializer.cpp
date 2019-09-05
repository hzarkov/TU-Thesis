#include "RoutingSpecializer.hpp"
#include "Logger.hpp"

RoutingSpecializer::RoutingSpecializer(std::shared_ptr<NetworkFactory> nm)
:Plugin("Routing Specializer", nm)
{
   
}

void RoutingSpecializer::configure(Plugin::Configuration_t conf)
{
    for(auto configuration : conf)
    {
        std::string interface_name = configuration.first;
        std::shared_ptr<TrafficSpecialization> traffic_specialization;
        try
        {
            traffic_specialization = this->traffic_specializations.at(interface_name);
        }
        catch(std::exception& e)
        {
            traffic_specialization = nullptr;
        }
        if(nullptr == traffic_specialization)
        {
            try
            {
                traffic_specialization = std::make_shared<TrafficSpecialization>(this->network_manager, interface_name);
                this->traffic_specializations[interface_name] = traffic_specialization;
            }
            catch(std::exception& e)
            {
                WarningLogger << "Failed to add interface_name configuration. " << e.what() << std::endl;
                this->traffic_specializations[interface_name] = traffic_specialization;
                continue;
            }
        }
        std::stringstream ss(configuration.second);
        std::string destination;
        while (std::getline(ss, destination, ',')) 
        {
            if(1) //ToDo: Check if destination is IP or domain
            {
                try
                {
                    traffic_specialization->addIP(destination);
                }
                catch(std::exception& e)
                {
                    ErrorLogger << e.what() << std::endl;
                }
            }
            else if(1)
            {
                traffic_specialization->addDomain(destination);
            }
            else
            {
                 WarningLogger << "Unknown domain type of '" + destination << "'" << std::endl;
            } 
        }
    }   
}

Plugin::Configuration_t RoutingSpecializer::getConfiguration()
{
    Plugin::Configuration_t result;
    for(auto traffic_specialization : this->traffic_specializations)
    {
        result[traffic_specialization.first] = "";
        if(nullptr != traffic_specialization.second)
        {
            for(auto ip : traffic_specialization.second->getIPs())
            {
                result[traffic_specialization.first] += ip + ",";
            }
        }
    }
    return result;
}

void RoutingSpecializer::exec()
{

}

extern "C"
{
    Plugin* allocator(std::shared_ptr<NetworkFactory> nm)
    {
        return new RoutingSpecializer(nm);
    }
}