#ifndef PING_INSTERNER_SWITCHER
#define PING_INSTERNER_SWITCHER

#include "InternetSwitcher.hpp"
#include "InterfaceController.hpp"

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

class PingInternetSwitcher : public InternetSwitcher
{
public:
    PingInternetSwitcher(std::shared_ptr<NetworkManager> nm, std::vector<std::string> interfaces);
    ~PingInternetSwitcher();
    void start();
    void stop();
private:
    void Run();

    std::mutex thread_mutex;
    bool running;
    std::thread thread;
    std::condition_variable thread_block;
    std::mutex thread_block_mutex;

    std::vector<std::shared_ptr<InterfaceController>> interfaces;
};
#endif