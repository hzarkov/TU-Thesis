#ifndef ROUTING_SPECIALIZER_HPP
#define ROUTING_SPECIALIZER_HPP

#include "Plugin.hpp"
#include "TrafficSpecialization.hpp"

class RoutingSpecializer : public Plugin
{
public:
    RoutingSpecializer(std::shared_ptr<NetworkFactory> nm);  
    ~RoutingSpecializer() = default;
    void configure(Configuration_t conf);
    void exec();
    Configuration_t getConfiguration();
private:
    std::map<std::string, std::shared_ptr<TrafficSpecialization>> traffic_specializations;
};
#endif