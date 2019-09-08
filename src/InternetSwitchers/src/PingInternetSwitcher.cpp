#include "PingInternetSwitcher.hpp"
#include "System.hpp"
#include "Logger.hpp"
#include "ElapsedTime.hpp"

#include <chrono>

const std::string PING_ADDRESS = "8.8.8.8";
PingInternetSwitcher::PingInternetSwitcher(std::shared_ptr<NetworkFactory> nm)
:Plugin("Ping Internet Switcher",nm)
{
    
}

void PingInternetSwitcher::configure(Plugin::Configuration_t conf)
{
    std::vector<std::string> interfaces_result;
    std::stringstream ss(conf["interfaces"]);
    std::string interface_name;
    while (std::getline(ss, interface_name, ',')) 
    {
        try
        {
            interfaces_result.push_back(interface_name);
        }
        catch(std::exception& e)
        {
            WarningLogger << "Failed to load " << interface_name << ": " << e.what() << std::endl;
        }
    }
    std::lock_guard<std::mutex> interfaces_mutex_lock(this->interfaces_mutex);
    this->interfaces.clear();
    this->interfaces = interfaces_result;
}

Plugin::Configuration_t PingInternetSwitcher::getConfiguration()
{
    std::lock_guard<std::mutex> interfaces_mutex_lock(this->interfaces_mutex);
    Plugin::Configuration_t result;
    result["interfaces"] = "";
    for(auto interface_name : this->interfaces)
    {
        result["interfaces"] += interface_name + ",";
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
    System::call("echo 1 > /proc/sys/net/ipv4/ip_forward");
    std::shared_ptr<Route> default_gw;
    while(this->running)
    {
        //ElapsedTime interface_pick_time, other;
        //interface_pick_time.start();
        //InformationLogger << "Making a copy of interfaces vector" << std::endl;
        //other.start();
        std::vector<std::string> interfaces_copy;
        {
            std::lock_guard<std::mutex> interfaces_mutex_lock(this->interfaces_mutex);
            interfaces_copy = this->interfaces;     
        }
        //InformationLogger << "Copy is ready, time=" << other.ready() << std::endl;
        for(auto interface_name: interfaces_copy)
        {
            try
            {
                //InformationLogger << "Starting ping check" << std::endl;
                //other.start();
                std::shared_ptr<InterfaceController> interface = this->network_manager->getInterface(interface_name);
                std::shared_ptr<Route> ping_route = this->network_manager->addRoute(PING_ADDRESS + "/32", interface->getGW());
                int ping_result = System::call("ping -W 1 -c 1 " + PING_ADDRESS + " > /dev/null");
                ping_route = nullptr;
                //InformationLogger << "Ping check is ready, time= " << other.ready() << std::endl;
                if(0 == ping_result)
                {
                    if(interface_name != this->current_interface)
                    {
                        //InformationLogger << "Setting up ips" << std::endl;
                        //other.start();
                        try
                        {
                            default_gw = nullptr;
                            default_gw = this->network_manager->addRoute("0.0.0.0/0", interface->getGW());
                            this->current_interface = interface_name;
                            this->masquarade_chain = this->network_manager->getXTable("nat")->getChain("POSTROUTING");
                            this->masquarade_chain->addRule("--out-interface " + interface_name + " -j MASQUERADE");
                            System::call("sysctl net.ipv4.conf." + interface_name + ".rp_filter=2");
                            std::vector<DNSMasqConfiguration::Configuration_t> resolv_conf;
                            for(std::string dns_server : interface->getDNSServers())
                            {
                                auto pair = std::make_pair("server", dns_server);
                                resolv_conf.push_back(pair);
                            }
                            this->dns_configaration = this->network_manager->getDNSMasqController()->addConfiguration("resolvconf", resolv_conf);
                            this->network_manager->getDNSMasqController()->reloadConfiguration();
                            InformationLogger << "Internet source changed to " << this->current_interface << " interface." << std::endl;
                        }
                        catch(std::exception& e)
                        {
                            ErrorLogger << "Failed to setup interface('" << interface_name << "') because of ' " << e.what() << "' error." << std::endl;
                        }
                        //InformationLogger << "Internet source was configured as default, time= " << other.ready() << std::endl;
                    }
                    break;
                }
            }
            catch(std::exception& e)
            {
                WarningLogger << "Failed to check internet connection of " << interface_name << ": " << e.what() << std::endl;
            }
        }
        //InformationLogger << "Internet source(" << this->current_interface << ") was picked, time = " << interface_pick_time.ready() << std::endl;
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