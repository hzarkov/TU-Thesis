#ifndef WEB_PLUGIN_CONFIGURATOR_HPP
#define WEB_PLUGIN_CONFIGURATOR_HPP

#include "PluginConfigurator.hpp"

class WebPluginConfigurator : public PluginConfigurator
{
public:
    WebPluginConfigurator() = default;
    void start();
    void stop();
    ~WebPluginConfigurator() = default;
};
#endif