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
};
#endif