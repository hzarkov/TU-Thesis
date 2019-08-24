#include "PluginConfigurator.hpp"

void PluginConfigurator::addPlugin(std::shared_ptr<Plugin> plugin)
{
    this->plugins.push_back(plugin);
}