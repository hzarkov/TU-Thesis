#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include "NetworkManager.hpp"
#include <memory>

class Plugin
{
protected:
    std::shared_ptr<NetworkManager> network_manager;
public:
    Plugin(std::shared_ptr<NetworkManager> network_manager);
    virtual ~Plugin() = default;
    virtual void exec() = 0;
    virtual void configure(std::map<std::string, std::string> configuration) = 0;
};

extern "C"
{
    Plugin* allocator(std::shared_ptr<NetworkManager> nm);
    void deallocator(Plugin* p);
}
#endif