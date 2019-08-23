#include "PingInternetSwitcher.hpp"
#include "System.hpp"
#include "Logger.hpp"

#include <chrono>

const std::string PING_ADDRESS = "8.8.8.8";
PingInternetSwitcher::PingInternetSwitcher(std::shared_ptr<NetworkManager> nm, std::vector<std::string> interface_names)
:InternetSwitcher(nm)
{
    for(auto interface_name: interface_names)
    {
        std::shared_ptr<InterfaceController> interface;
        try
        {
            interface = this->network_manager->getInterface(interface_name);
            this->interfaces.push_back(interface);
        }
        catch(std::exception& e)
        {
            WarningLogger << "Failed to load " << interface_name << ": " << e.what() << std::endl;
        }
    }
}

void PingInternetSwitcher::start()
{
    std::lock_guard<std::mutex> thread_mutex_lock(this->thread_mutex);
    if(false == this->running)
    {            
        this->running = true;
        this->thread = std::thread(&PingInternetSwitcher::Run, this);
    }
}

void PingInternetSwitcher::stop()
{
    std::lock_guard<std::mutex> thread_mutex_lock(this->thread_mutex);
    if(false != this->running)
    {
        this->running = false;
        this->thread_block.notify_all();
        this->thread.join();
    }
}

void PingInternetSwitcher::Run()
{
    std::shared_ptr<Route> default_gw;
    while(this->running)
    {
        for(auto interface: this->interfaces)
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
    this->stop();
}

extern "C"
{
    InternetSwitcher* allocator(std::shared_ptr<NetworkManager> nm, std::vector<std::string> interfaces)
    {
        return new PingInternetSwitcher(nm, interfaces);
    }

    void deallocator(InternetSwitcher* p)
    {
        delete p;
    }
}