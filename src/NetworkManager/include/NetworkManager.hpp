#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP
#include "InterfaceController.hpp"
#include "Route.hpp"
#include "ChainRule.hpp"
#include "PackageRule.hpp"
#include "DNSMasqController.hpp"
#include "DHCPServer.hpp"

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

class NetworkManager
{
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

    std::vector<std::weak_ptr<ChainRule>> chain_rules;
    std::mutex chain_rules_mutex;

    std::vector<std::weak_ptr<PackageRule>> package_rules;
    std::mutex package_rules_mutex;

    std::shared_ptr<DNSMasqController> dnsmasq_controller;
    std::shared_ptr<DHCPServer> dhcp_server;
    // iptables rule list
    void addInterface(std::string interface_name);
    void removeInterface(std::string interface_name);
    std::vector<std::string> getSystemInterfacesList();
    void checkerThread();
    
public:
    NetworkManager();
    ~NetworkManager();
    void start();
    void stop();
    std::shared_ptr<InterfaceController> getInterface(std::string interface_name);
    std::shared_ptr<Route> addRoute(std::string destination, std::string gateway="0.0.0.0", std::string interface_name="", int metric=0);
    std::shared_ptr<PackageRule> addPackageRule(std::string rule, std::string type="-A");
    std::shared_ptr<ChainRule> addChainRule(std::string rule);
};

#endif