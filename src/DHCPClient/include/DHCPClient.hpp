#ifndef DHCP_CLIENT_HPP
#define DHCP_CLIENT_HPP
#include <vector>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class DHCPClient
{
private:
    std::thread thread;
    std::mutex running_mutex;
    bool running;
    std::mutex thread_timer_mutex;
    std::condition_variable thread_timer_cond;
    std::function<void (std::string, std::string, std::string, std::vector<std::string>)> callback;

    std::mutex trigger_cond_mutex;
    std::condition_variable trigger_cond;

    std::string interface;
    std::string dhcp_server;
    std::string ip;
    std::string mask;
    std::string broadcast;
    std::string gateway;
    std::vector<std::string> dns_servers;
    uint lease;
    void run();
public:
    DHCPClient(std::string interface);
    ~DHCPClient();
    void start();
    void stop();
    void triggerIPCheck();
    void registerConfigCallback(std::function<void (std::string, std::string, std::string, std::vector<std::string>)> callback);
    /*std::string getIP();
    std::string getMask();
    std::string getGateway();
    std::string getDNS();*/
};
#endif  