#include "DHCPClient.hpp"
#include "System.hpp"
#include <sstream>
void DHCPClient::run()
{
    while(this->running)
    {
        std::string dhcp_result = System::exec("./dhcpresolver -T " + this->interface); //ToDo: Add graceful termination for dhcpclient because now it it's blocked it would never end.
        if(!dhcp_result.empty())
        {
            std::stringstream lineStream(dhcp_result);
            std::getline(lineStream, this->dhcp_server, ',');
            std::getline(lineStream, this->ip, ',');
            std::getline(lineStream, this->mask, ',');
            std::getline(lineStream, this->broadcast, ',');
            std::getline(lineStream, this->gateway, ',');
            std::string lease_string;
            std::getline(lineStream, lease_string, ',');
            this->lease = std::stoi(lease_string);
            std::string dns_server;
            while(std::getline(lineStream, dns_server, ','))
            {
                this->dns_servers.push_back(dns_server);
            }
            this->callback(this->ip, this->mask, this->gateway, this->dns_servers);
        }
        this->trigger_cond.notify_all();
        if(this->running)
        {
            std::unique_lock<std::mutex> lk(this->thread_timer_mutex);
            this->thread_timer_cond.wait_for(lk,std::chrono::seconds(this->lease-(this->lease/10)));
        }
    }
}

DHCPClient::DHCPClient(std::string interface)
:interface(interface),running(false)
{

}

DHCPClient::~DHCPClient()
{
    this->stop();
}

void DHCPClient::triggerIPCheck()
{
    this->thread_timer_cond.notify_all();
    std::unique_lock<std::mutex> lk(this->trigger_cond_mutex);
    this->trigger_cond.wait_for(lk,std::chrono::seconds(2));
}

void DHCPClient::start()
{
    std::lock_guard<std::mutex> running_mutex_guard(this->running_mutex);
    if(!this->running)
    {
        this->running = true;
        this->thread = std::thread(&DHCPClient::run,this);
        std::unique_lock<std::mutex> lk(this->trigger_cond_mutex);
        this->trigger_cond.wait_for(lk,std::chrono::seconds(2));
    }
}

void DHCPClient::stop()
{
    std::lock_guard<std::mutex> running_mutex_guard(this->running_mutex);
    if(this->running)
    {
        this->running = false;
        this->triggerIPCheck();
        this->thread.join();
    }
}

void DHCPClient::registerConfigCallback(std::function<void (std::string, std::string, std::string, std::vector<std::string>)> callback)
{
    this->callback = callback;
}

/*std::string DHCPClient::getIP()
{
    return this->ip;
}

std::string DHCPClient::getMask()
{
    return this->mask;
}

std::string DHCPClient::getGateway()
{
    return this->gateway;
}

std::string DHCPClient::getDNSServers()
{
    return this->dns_servers;
}
*/