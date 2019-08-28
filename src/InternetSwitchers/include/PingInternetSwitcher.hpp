#ifndef PING_INSTERNER_SWITCHER
#define PING_INSTERNER_SWITCHER

#include "Plugin.hpp"
#include "InterfaceController.hpp"

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

class PingInternetSwitcher : public Plugin
{
public:
    PingInternetSwitcher(std::shared_ptr<NetworkFactory> nm);
    ~PingInternetSwitcher();
    void configure(Configuration_t conf);
    void exec();
    Configuration_t getConfiguration();
private:
    void Run();

    std::mutex thread_mutex;
    bool running;
    std::thread thread;
    std::condition_variable thread_block;
    std::mutex thread_block_mutex;

    std::vector<std::string> interfaces;
    std::mutex interfaces_mutex;
};
#endif