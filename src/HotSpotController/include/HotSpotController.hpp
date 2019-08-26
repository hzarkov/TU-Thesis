#ifndef HOTSPOT_CONTROLLER
#define HOTSPOT_CONTROLLER

#include <string>

class HotSpotController
{
public:
    HotSpotController(std::string interface);
    void addConfigurationFile(std::string ssid, std::string password);
    void start();
    void stop();
private:
    getConfigurationFile();
    getPidFile();
    std::string interface_name;
};
#endif