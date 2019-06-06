#ifndef INTERFACE_HPP
#define INTERFACE_HPP
#include<vector>
#include<functional>
#include<map>
#include<string>

#include "IP.hpp"

class Interface
{
private:
    std::string interface_name;
    bool status;
    IP ip;
    IP mask;
    IP gateway;
    std::vector<IP> dns_servers;
    std::map<int,std::function<void(IP gateway)>> gateway_change_callback;
    std::map<int,std::function<void(IP ip, IP mask)>> ip_change_callback;
    std::map<int,std::function<void(std::vector<IP> dns_servers)>> dns_change_callback;
    std::map<int,std::function<void(bool status)>> status_change_callback;

    template<typename T, typename... DataT>
    void callCallbacksOf(T callback_list, DataT... data);

    template<typename callbackList, typename callback>
    inline int registerCallback(callbackList &list, callback &cb);
public:
    Interface(std::string interface_name);
    void enable();
    void disable();
    
    void setIP(IP ip, IP mask);
    void setGW(IP ip);
    void setDNS(std::vector<IP> ips);

    int registerGatewayCallback(std::function<void(IP gateway)> callback);
    int registerIPCallback(std::function<void(IP ip, IP mask)> callback);
    int registerDNSCallback(std::function<void(std::vector<IP> dns_servers)> callback);
    int registerStatusCallback(std::function<void(bool status)> callback);
};

#endif