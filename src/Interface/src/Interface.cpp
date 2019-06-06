#include<stdexcept>
#include<future>

#include "Interface.hpp"
#include "System.hpp"

template<typename callbackList, typename callback>
int Interface::registerCallback(callbackList &change_callback_list, callback &cb)
{
    static int id = 0;
    change_callback_list[id] = cb;
    return id++;
}

template<typename T, typename... DataT>
void Interface::callCallbacksOf(T callback_list, DataT... data)
{
    for(auto callback : callback_list)
    {
        std::thread t(callback.second, data...);
        t.detach(); //ToDo: No way to wait for callbacks to end before end;
    }
}

Interface::Interface(std::string interface_name)
:interface_name(interface_name)
{
}

void Interface::enable()
{
    if( System::call("ifconfig " + this->interface_name + " up") )
    {
        throw std::runtime_error("Failed to disable the interface");
    }
    this->status = true;
    this->callCallbacksOf(this->status_change_callback, true);
}

void Interface::disable()
{
    if( System::call("ifconfig " + this->interface_name + " down") )
    {
        throw std::runtime_error("Failed to disable the interface");
    }
    this->status = false;
    this->callCallbacksOf(this->status_change_callback, false);
}

void Interface::setIP(IP ip, IP mask=IP("255.255.255.255"))
{
    if( System::call("ifconfig " + this->interface_name + " " + ip + " netmask " + mask))
    {
        throw std::runtime_error("Failed to setup the interface");
    }
    this->ip = ip;
    this->mask = mask;
    this->callCallbacksOf(this->ip_change_callback, this->ip, this->mask);
}

void Interface::setGW(IP ip)
{
    this->gateway = ip;
    this->callCallbacksOf(this->gateway_change_callback, this->gateway);
}

void Interface::setDNS(std::vector<IP> dns_servers)
{
    this->dns_servers = dns_servers;
    this->callCallbacksOf(this->dns_change_callback, this->dns_servers);
}

int Interface::registerGatewayCallback(std::function<void(IP gateway)> callback)
{
    return this->registerCallback(this->gateway_change_callback, callback);
}

int Interface::registerIPCallback(std::function<void(IP ip, IP mask)> callback)
{
    return this->registerCallback(this->ip_change_callback, callback);
}

int Interface::registerDNSCallback(std::function<void(std::vector<IP> dns_servers)> callback)
{
    return this->registerCallback(this->dns_change_callback, callback);
}

int Interface::registerStatusCallback(std::function<void(bool status)> callback)
{
    return this->registerCallback(this->status_change_callback, callback);
}

