#include "PluginConfigurator.hpp"

void PluginConfigurator::addPlugin(std::shared_ptr<Plugin> plugin)
{
    static uint id = 0;
    this->plugins[id++] = plugin;
}