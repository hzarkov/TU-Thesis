#ifndef ROUTING_SPECIALIZER_HPP
#define ROUTING_SPECIALIZER_HPP

#include "TrafficSpecializer.hpp"
#include "TrafficSpecialization.hpp"

class RoutingSpecializer : public TrafficSpecializer
{
public:
    RoutingSpecializer(std::shared_ptr<NetworkManager> nm, std::string interface, std::vector<std::string> route_domains);  
    ~RoutingSpecializer() = default;
private:
    std::unique_ptr<TrafficSpecialization> traffic_specialization;
};

extern "C"
{
    TrafficSpecializer* allocator(std::shared_ptr<NetworkManager> nm, std::string interface, std::vector<std::string> route_domains);
    void deallocator(TrafficSpecializer* p);
}
#endif