#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include "NetworkFactory.hpp"
#include <memory>
#include <map>
#include <string>

class Plugin
{
public:
    typedef std::map<std::string, std::string> Configuration_t;
    
    Plugin(std::string name, std::shared_ptr<NetworkFactory> network_manager);

    virtual ~Plugin() = default;
    virtual void exec() = 0;
    virtual void configure(Configuration_t configuration) = 0;
    virtual Configuration_t getConfiguration() = 0;
    std::string getName();
protected:
    std::shared_ptr<NetworkFactory> network_manager;
    std::string name;
};

extern "C"
{
    Plugin* allocator(std::shared_ptr<NetworkFactory> nm);
    void deallocator(Plugin* p);
}
#endif