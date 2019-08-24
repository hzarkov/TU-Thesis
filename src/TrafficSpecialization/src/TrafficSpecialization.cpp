#include "TrafficSpecialization.hpp"
#include <stdexcept>

const std::string ipset_name_extension = "_ipset";
TrafficSpecialization::TrafficSpecialization(std::shared_ptr<NetworkFactory> nm, std::string interface_name)
{
    std::shared_ptr<InterfaceController> interface;
    try
    {
        interface = nm->getInterface(interface_name);
    }
    catch(std::out_of_range& e)
    {
        throw std::invalid_argument("Unknown '" + interface_name + "' interface.");
    }
    static int mark = 0;
    mark++;
    uint routing_table_id = 100+mark;
    std::string ipset_name = interface_name + ipset_name_extension;
    this->route_rule = nm->getRouteRule(routing_table_id, routing_table_id, "fwmark " + std::to_string(mark));
    this->ipset = nm->getIPSet(interface_name + ipset_name_extension, "hash:ip");
    this->routes.push_back(nm->addRoute("0.0.0.0/0", interface->getGW(), interface_name, 0, std::to_string(routing_table_id)));
    //this->route = nm->getRoute(std::string destination, std::string gateway, std::string interface_name, int metric, std::string table);
    std::string marking_rule = "-m set --match-set " + ipset_name + " dst -m mark --mark 0 -j MARK --set-mark " + std::to_string( mark );

    this->mangle_prerouting_chain = nm->getXTable("mangle")->getChain("PREROUTING");
    this->mangle_output_chain = nm->getXTable("mangle")->getChain("OUTPUT");

    this->mangle_prerouting_chain->addRule(marking_rule);
    this->mangle_output_chain->addRule(marking_rule);
}

void TrafficSpecialization::addIP(std::string ip)
{
    this->ipset->addIP(ip);
}

void TrafficSpecialization::addDomain(std::string domain_name)
{

}

std::vector<std::string> TrafficSpecialization::getIPs()
{
    return this->ipset->getIPs();
}

std::vector<std::string> TrafficSpecialization::getDomains()
{
    std::vector<std::string> result;
    return result;
}