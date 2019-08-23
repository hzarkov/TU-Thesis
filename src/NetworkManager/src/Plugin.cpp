#include "Plugin.hpp"
#include "Logger.hpp"

Plugin::Plugin(std::shared_ptr<NetworkManager> nm)
:network_manager(nm)
{
    
}

extern "C"
{
    void deallocator(Plugin* p)
    {
        delete p;
    }
}