#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include "NetworkFactory.hpp"
#include <memory>

class Plugin
{
public:
    typedef std::map<std::string, std::string> Configuration_t;
    
    Plugin(std::shared_ptr<NetworkFactory> network_manager);
    void config(Configuration_t configuration);
    Configuration_t getConfFields();

    virtual ~Plugin() = default;
    virtual void exec() = 0;
    virtual void configure(Configuration_t configuration) = 0;
    virtual Configuration_t getConfiguration() = 0;
protected:
    std::shared_ptr<NetworkFactory> network_manager;
};

extern "C"
{
    Plugin* allocator(std::shared_ptr<NetworkFactory> nm);
    void deallocator(Plugin* p);
}
#endif