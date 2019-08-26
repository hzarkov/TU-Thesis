
WirelessController::WirelessController(std::string interface_name)
:InterfaceController(interface_name)
{

}

void WirelessController::connectTo(WirelessNetwork network)
{

}

void WirelessController::disconnect()
{
    if(hotspot_controller.isEnabled())
    {
        hotspot_controller.stop();
    }
}

void WirelessController::setupHotSpot(std::string name, std::string password)
{
    hotspot_controller.start(this->getName(), name, password);
}

std::vector<WirelessNetwork> WirelessController::getAvailableNetworks()
{
    std::vector<WirelessNetwork> result;
    return result;
}

