#include "NetworkFactory.hpp"
#include "Logger.hpp"
#include "EthernetController.hpp"

#include <chrono>
#include <algorithm>
#include <dirent.h>

NetworkFactory::NetworkFactory()
:dnsmasq_controller(std::make_shared<DNSMasqController>()),
dhcp_server(std::make_shared<DHCPServer>(dnsmasq_controller))
{
    this->xtables["filter"] = std::make_shared<XTables::Table>("filter");
    this->xtables["nat"] = std::make_shared<XTables::Table>("nat");
    this->xtables["mangle"] = std::make_shared<XTables::Table>("mangle");
}

NetworkFactory::~NetworkFactory()
{

}

void NetworkFactory::start()
{
    /*this->checker_running = true;
    this->checker_thread = std::thread(&NetworkFactory::checkerThread, this);
    std::unique_lock<std::mutex> lk(checker_thread_start_mutex);
    checker_thread_start_cond.wait(lk);*/
    this->checkerThread();
}

void NetworkFactory::stop()
{
    /*this->checker_running = false;
    this->checker_thread_timer_cond.notify_one();
    this->checker_thread.join();*/
}

void NetworkFactory::checkerThread()
{
    //bool notify_start = false;
    //while(this->checker_running) // for now use only on start
    //{
        //bool change = false;
        std::vector<std::string> interfaces_list = this->getSystemInterfacesList();
        /*std::map<std::string, std::shared_ptr<InterfaceController>> current_nm_interfaces = this->interfaces;
        
        for(auto interface : current_nm_interfaces)
        {
            auto found = std::find(interfaces_list.begin(), interfaces_list.end(), interface.first);
            if(interfaces_list.end() == found)
            {
                std::lock_guard<std::mutex> interfaces_mutex_lock(this->interfaces_mutex);
                this->interfaces.erase(interface.first);
                //change = true;
            }
        }*/

        for(auto interface_name : interfaces_list)
        {
            try
            {
                this->addInterface(interface_name);
                //change = true;
            }
            catch(std::runtime_error& e)
            {
                WarningLogger << "Failed to add " << interface_name << ": " << e.what() << std::endl;
            }
        }

        /*if(change)
        {
            //ToDo: Notify configurators for the change.
        }

        if(!notify_start)
        {
            this->checker_thread_start_cond.notify_one();
            notify_start = true;
        }
        if(this->checker_running)
        {
            std::unique_lock<std::mutex> lk(this->checker_thread_timer_mutex);
            this->checker_thread_timer_cond.wait_for(lk,std::chrono::seconds(5));
        }*/
    //}
}

std::vector<std::string> NetworkFactory::getSystemInterfacesList()
{
    std::vector<std::string> result;
    std::string interfaces_path = "/sys/class/net";
    struct dirent *entry = nullptr;
    DIR *dp = nullptr;
    dp = opendir(interfaces_path.c_str());
    if (dp != nullptr) {
        while ((entry = readdir(dp)))
        {
            DebugLogger << "Interface name from file: " << entry->d_name << std::endl;
            result.push_back(entry->d_name);
        }
    }

    closedir(dp);
    return result;
}

void NetworkFactory::addInterface(std::string interface_name) // add callback for in case of removal
{
    std::string interface_type = interface_name.substr(0,2);
    if(interface_type == "en" || interface_type == "et" || interface_type == "us" || interface_type == "lo")
    {
        std::lock_guard<std::mutex> interfaces_mutex_lock(this->interfaces_mutex);
        this->interfaces[interface_name] = std::make_shared<EthernetController>(interface_name, dhcp_server);
    }
    else
    {
        throw std::runtime_error("Unknown interface type");
    }
}

std::shared_ptr<DNSMasqController> NetworkFactory::getDNSMasqController()
{
    return this->dnsmasq_controller;
}

std::shared_ptr<InterfaceController> NetworkFactory::getInterface(std::string interfaces_name)
{
    std::lock_guard<std::mutex> interfaces_mutex_lock(this->interfaces_mutex);
    return this->interfaces.at(interfaces_name);
}

std::shared_ptr<Route> NetworkFactory::addRoute(std::string destination, std::string gateway, std::string interface_name, int metric, std::string table)
{
    std::lock_guard<std::mutex> routes_mutex_lock(this->routes_mutex);
    std::shared_ptr<Route> result;
    auto find_function = [destination, metric, table](std::weak_ptr<Route> elem){
         auto element = elem.lock();
        return element->getDestination() == destination && element->getMetric() == metric && element->getTable() == table;
    };
    result = this->getWeakElement<Route>(this->routes, find_function, destination, gateway, interface_name, metric, table);
    return result;
}

std::shared_ptr<RouteRule> NetworkFactory::getRouteRule(uint priority, uint lookup, std::string additional_data)
{
    std::lock_guard<std::mutex> route_rules_mutex_lock(this->route_rules_mutex);
    std::shared_ptr<RouteRule> result;
    auto find_function = [priority, lookup](std::weak_ptr<RouteRule> elem){
         auto element = elem.lock();
        return element->getPriority() == priority && element->getLookup() == lookup;
    };
    result = this->getWeakElement<RouteRule>(this->route_rules, find_function, priority, lookup, additional_data);
    return result;
}

std::shared_ptr<IPSet> NetworkFactory::getIPSet(std::string name, std::string type)
{
    std::lock_guard<std::mutex> ipsets_mutex_lock(this->ipsets_mutex);
    std::shared_ptr<IPSet> result;
    auto find_function = [name](std::weak_ptr<IPSet> elem){
        auto element = elem.lock();
        return element->getName() == name;
    };
    result = this->getWeakElement<IPSet>(this->ipsets, find_function, name, type);
    return result;
}

std::shared_ptr<XTables::Table> NetworkFactory::getXTable(std::string name)
{
    return this->xtables.at(name);
}

template<typename T, typename Array_t, typename ...Args>
std::shared_ptr<T> NetworkFactory::getWeakElement(Array_t array, std::function<bool (std::weak_ptr<T> elem)> find_function, Args... args)
{
    std::shared_ptr<T> result;
    std::remove_if(array.begin(),array.end(), [](std::weak_ptr<T> elem){
        return elem.expired();
    });

    auto found = std::find_if(array.begin(),array.end(), find_function);

    if(array.end() != found)
    {
        result = found->lock();
    }
    else
    {
        std::shared_ptr<T> new_element = std::make_shared<T>(args...);
        array.push_back(new_element);
        result = new_element;
    }
    return result;
}