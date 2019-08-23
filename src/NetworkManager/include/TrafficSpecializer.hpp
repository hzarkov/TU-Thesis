#ifndef TRAFFIC_SPECIALIZER_HPP
#define TRAFFIC_SPECIALIZER_HPP
#include "Plugin.hpp"

class TrafficSpecializer : public Plugin
{
public:
    TrafficSpecializer(std::shared_ptr<NetworkManager> nm);
    void exec();
    virtual ~TrafficSpecializer() = default;
};
#endif