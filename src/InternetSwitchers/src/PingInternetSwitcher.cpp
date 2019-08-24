#include "PingInternetSwitcher.hpp"
#include "System.hpp"
#include "Logger.hpp"

#include <chrono>

const std::string PING_ADDRESS = "8.8.8.8";
PingInternetSwitcher::PingInternetSwitcher(std::shared_ptr<NetworkFactory> nm)
:Plugin(nm)
{
    
}

void PingInternetSwitcher::configure(Plugin::Configuration_t conf)
{
    std::stringstream ss(conf["interfaces"]);
    std::string interface_name;
    while (std::getline(ss, interface_name, ',')) 
    {
        std::shared_ptr<InterfaceController> interface;
        try
        {
            interface = this->network_manager->getInterface(interface_name);
            {
                std::lock_guard<std::mutex> interfaces_mutex_lock(this->interfaces_mutex);
                this->interfaces.push_back(interface);
            }
        }
        catch(std::exception& e)
        {
            WarningLogger << "Failed to load " << interface_name << ": " << e.what() << std::endl;
        }
    }
}

Plugin::Configuration_t PingInternetSwitcher::getConfiguration()
{
    std::lock_guard<std::mutex> interfaces_mutex_lock(this->interfaces_mutex);
    Plugin::Configuration_t result;
    result["interfaces"] = "";
    for(auto interface : this->interfaces)
    {
        result["interfaces"] += interface->getName() + ",";
    }
    return result;
}

void PingInternetSwitcher::exec()
{
    std::lock_guard<std::mutex> thread_mutex_lock(this->thread_mutex);
    if(false == this->running)
    {            
        this->running = true;
        this->thread = std::thread(&PingInternetSwitcher::Run, this);
    }
}

void PingInternetSwitcher::Run()
{
    std::shared_ptr<Route> default_gw;
    while(this->running)
    {
        std::vector<std::shared_ptr<InterfaceController>> interfaces_copy;
        {
            std::lock_guard<std::mutex> interfaces_mutex_lock(this->interfaces_mutex);
            interfaces_copy = this->interfaces;     
        } 
        for(auto interface: interfaces_copy)
        {
            try
            {
                std::shared_ptr<Route> route = this->network_manager->addRoute(PING_ADDRESS + "/32", interface->getGW());
                if(0 == System::call("ping -c " + PING_ADDRESS))
                {
                    default_gw = this->network_manager->addRoute("0.0.0.0/0", interface->getGW());
                }
            }
            catch(std::exception& e)
            {
                WarningLogger << "Failed to check internet connection of " << interface << ": " << e.what() << std::endl;
            }
        }
        std::unique_lock<std::mutex> lk(this->thread_block_mutex);
        this->thread_block.wait_for(lk, std::chrono::seconds(1));
    }
}

PingInternetSwitcher::~PingInternetSwitcher()
{
    std::lock_guard<std::mutex> thread_mutex_lock(this->thread_mutex);
    if(false != this->running)
    {
        this->running = false;
        this->thread_block.notify_all();
        this->thread.join();
    }
}

extern "C"
{
    Plugin* allocator(std::shared_ptr<NetworkFactory> nm)
    {
        return new PingInternetSwitcher(nm);
    }
}