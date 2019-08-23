#ifndef HOTSPOT_CONTROLLER
#define HOTSPOT_CONTROLLER
class HotSpotController
{
public:
    HotSpotController();
    void addConfigurationFile(std::string interface, std::string ssid, std::string password);
    void start();
    void stop();
};
#endif