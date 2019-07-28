#ifndef CONFIGURATOR_HPP
#define CONFIGURATOR_HPP

#include "Plugin.hpp"

#include <memory>

class Configurator : public Plugin
{
public:
    Configurator(std::shared_ptr<NetworkManager> nm);
    virtual ~Configurator() = default;
    virtual void configure() = 0;
    void exec();
};

#endif