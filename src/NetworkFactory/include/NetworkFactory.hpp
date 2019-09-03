#ifndef NETWORK_FACTORY_HPP
#define NETWORK_FACTORY_HPP
#include "InterfaceController.hpp"
#include "Route.hpp"
#include "Table.hpp"
#include "IPSet.hpp"
#include "RouteRule.hpp"
#include "DNSMasqController.hpp"
#include "DHCPServer.hpp"

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

class NetworkFactory
{    
public:
    NetworkFactory();
    ~NetworkFactory();
    void start();
    void stop();

    std::shared_ptr<DNSMasqController> getDNSMasqController();
    std::shared_ptr<InterfaceController> getInterface(std::string interface_name);
    std::shared_ptr<Route> addRoute(std::string destination, std::string gateway="0.0.0.0", std::string interface_name="", int metric=0, std::string table="");
    std::shared_ptr<XTables::Table> getXTable(std::string name);
    std::shared_ptr<RouteRule> getRouteRule(uint priority, uint lookup, std::string additional_data);
    std::shared_ptr<IPSet> getIPSet(std::string name, std::string type);
private:
    std::thread checker_thread;
    bool checker_running;
    std::condition_variable checker_thread_timer_cond;
    std::mutex checker_thread_timer_mutex;
    std::condition_variable checker_thread_start_cond;
    std::mutex checker_thread_start_mutex;

    std::map<std::string, std::shared_ptr<InterfaceController>> interfaces;
    std::mutex interfaces_mutex;

    std::vector<std::weak_ptr<Route>> routes;
    std::mutex routes_mutex;

    std::vector<std::weak_ptr<IPSet>> ipsets;
    std::mutex ipsets_mutex;

    std::vector<std::weak_ptr<RouteRule>> route_rules;
    std::mutex route_rules_mutex;

    std::map<std::string, std::shared_ptr<XTables::Table>> xtables;

    std::shared_ptr<DNSMasqController> dnsmasq_controller;
    std::shared_ptr<DHCPServer> dhcp_server;
    // iptables rule list
    void addInterface(std::string interface_name);
    void removeInterface(std::string interface_name);
    std::vector<std::string> getSystemInterfacesList();
    void checkerThread();

    template<typename T, typename Array_t, typename ...Args>
    std::shared_ptr<T> getWeakElement(Array_t array, std::function<bool (std::weak_ptr<T> elem)> find_function, Args... args);

};

#endif