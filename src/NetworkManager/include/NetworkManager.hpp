#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP
#include "InterfaceController.hpp"
#include "Route.hpp"

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
    std::map<int, std::shared_ptr<Route>> routes;
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
    //int getRoute();
};

#endif