#ifndef CONFIGURATOR_HPP
#define CONFIGURATOR_HPP

#include "NetworkManager.hpp"

#include <memory>

class Configurator
{
protected:
    std::shared_ptr<NetworkManager> netowrk_manager;
public:
    Configurator(std::shared_ptr<NetworkManager> nm);
    virtual ~Configurator() = default;
    virtual void configure() = 0;
};

#endif