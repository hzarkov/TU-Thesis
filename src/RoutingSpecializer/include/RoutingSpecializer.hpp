#ifndef ROUTING_SPECIALIZER_HPP
#define ROUTING_SPECIALIZER_HPP

#include "Plugin.hpp"
#include "TrafficSpecialization.hpp"

class RoutingSpecializer : public Plugin
{
public:
    RoutingSpecializer(std::shared_ptr<NetworkManager> nm);  
    ~RoutingSpecializer() = default;
    void configure(std::map<std::string, std::string> conf);
    void exec();
private:
    std::unique_ptr<TrafficSpecialization> traffic_specialization;
};
#endif