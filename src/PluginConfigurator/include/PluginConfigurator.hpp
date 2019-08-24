#ifndef PLUGIN_CONFIGURATOR_HPP
#define PLUGIN_CONFIGURATOR_HPP

#include "Plugin.hpp"

#include <memory>
#include <map>

class PluginConfigurator
{
public:
    PluginConfigurator() = default;
    void addPlugin(std::shared_ptr<Plugin> plugin);
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual ~PluginConfigurator() = default;
protected:
    std::map<int, std::shared_ptr<Plugin>> plugins;
};
#endif