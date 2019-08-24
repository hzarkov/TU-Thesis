#include "Plugin.hpp"
#include "Logger.hpp"

Plugin::Plugin(std::shared_ptr<NetworkFactory> nm)
:network_manager(nm)
{
    
}

void Plugin::config(Plugin::Configuration_t configuration)
{
    this->configure(configuration);
}

Plugin::Configuration_t Plugin::getConfFields()
{
    return this->getConfiguration();
}

extern "C"
{
    void deallocator(Plugin* p)
    {
        delete p;
    }
}