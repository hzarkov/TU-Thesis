#include "Plugin.hpp"
#include "Logger.hpp"

Plugin::Plugin(std::string name, std::shared_ptr<NetworkFactory> nm)
:name(name), network_manager(nm)
{
    
}

std::string Plugin::getName()
{
    return this->name;
}

extern "C"
{
    void deallocator(Plugin* p)
    {
        delete p;
    }
}