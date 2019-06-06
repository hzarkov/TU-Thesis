#include "Interface.hpp"
#include "Logger.hpp"
#include<string>
#include<chrono>
#include<thread>

void function(IP ip, IP mask)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    DebugLogger << "ip: " << ip << std::endl;
    DebugLogger << "mask: " << mask << std::endl;
}
void function1(IP gw)
{
    DebugLogger << "gw: " << gw << std::endl;
}
int main()
{
    Interface interface("enx503eaa8b4c8c");
    //interface.disable();
    interface.enable();
    interface.registerIPCallback(&function);
    interface.registerGatewayCallback(&function1);
    interface.setIP(IP("192.168.100.100"),IP("255.255.255.0"));
    interface.setGW(IP("192.168.100.1"));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return 0;
}