#ifndef TRAFFIC_SPECIALIZATION_HPP
#define TRAFFIC_SPECIALIZATION_HPP

#include "NetworkFactory.hpp"
#include <string>
#include <memory>

class TrafficSpecialization
{
public:
    TrafficSpecialization(std::shared_ptr<NetworkFactory> nm, std::string interface_name);
    void addIP(std::string ip);
    void addDomain(std::string domain_name);
    std::vector<std::string> getIPs();
    std::vector<std::string> getDomains();
    ~TrafficSpecialization() = default;
private:
    std::shared_ptr<RouteRule> route_rule;
    std::vector<std::shared_ptr<Route>> routes;
    std::shared_ptr<IPSet> ipset;
    std::shared_ptr<XTables::Chain> mangle_prerouting_chain;
    std::shared_ptr<XTables::Chain> mangle_output_chain;
};
#endif