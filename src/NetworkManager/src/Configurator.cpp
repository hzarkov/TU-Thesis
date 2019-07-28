#include "Configurator.hpp"

Configurator::Configurator(std::shared_ptr<NetworkManager> nm)
:Plugin(nm)
{
    
}

void Configurator::exec()
{
    this->configure();
}