#include "HotSpotController.hpp"

#include "System.hpp"

const std::string conf_file_path = "/etc/hostapd/hostapd.conf";

void HotSpotController::addConfigurationFile(std::string interface, std::string ssid, std::string password)
{
    std::ofsteam conf_file(conf_file_path);
    conf_file << "interface=" << interface << std::endl;
    conf_file << "driver=nl80211" << std::endl;
    conf_file << "ssid=" << name << std::endl;
    conf_file << "hw_mode=g" << std::endl;
    conf_file << "channel=6" << std::endl;
    conf_file << "macaddr_acl=0" << std::endl;
    conf_file << "auth_algs=1" << std::endl;
    conf_file << "ignore_broadcast_ssid=0" << std::endl;
    conf_file << "wpa=3" << std::endl;
    conf_file << "wpa_passphrase=" << password << std::endl;
    conf_file << "wpa_key_mgmt=WPA-PSK" << std::endl;
    conf_file << "wpa_pairwise=TKIP" << std::endl;
    conf_file << "rsn_pairwise=CCMP" << std::endl;
}

void HotSpotController::start()
{
    if(0 != System::call("systemctl start hostapd"))
    {
        throw std::runtime_error("Failed to start hostapd");
    }
}

void HotSpotController::stop()
{
    if(0 != System::call("systemctl stop hostapd"))
    {
        throw std::runtime_error("Failed to stop hostapd");
    }
}

void HotSpotController::reload()
{
    if(0 != System::call("systemctl reload hostapd"))
    {
        throw std::runtime_error("Failed to stop hostapd");
    }
}
