#ifndef WIRELESS_CONTROLLER_HPP
#define WIRELESS_CONTROLLER_HPP

#include "InterfaceController.hpp"
#include "HotSpotController.hpp"

class WirelessController: public InterfaceController
{
public:
    typedef struct WirelessNetwork_s
    {
        std::string name;
        std::string security_type;
        std::string pass;
    } WirelessNetwork;

    WirelessController(std::string interface_name);
    void connectTo(WirelessNetwork network);
    void disconnect();
    void setupHotSpot(std::string name, std::string password);
    std::vector<WirelessNetwork> getAvailableNetworks();
private:
    WirelessNetwork network;
    HotSpotController hotspot_controller;
};
#endif