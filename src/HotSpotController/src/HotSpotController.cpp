#include "HotSpotController.hpp"

#include "System.hpp"

const std::string conf_file_path = "/etc/hostapd/";
const std::string conf_file_extension = ".conf";

const std::string pid_file_path = "/var/run/";
const std::string pid_file_extension = ".pid";

const std::string file_service_extension = "_hotspot";


HotSpotController::HotSpotController(std::string interface_name)
:interface_name(inteface_name)
{

}

std::string getPidFile()
{
    return pid_file_pat + this->interface_name + file_service_extension + pid_file_extension;
}

std::string getConfigurationFile()
{
    return conf_file_path + this->interface_name + file_service_extension + conf_file_extension;
}

void HotSpotController::addConfigurationFile(std::string ssid, std::string password)
{
    std::ofsteam conf_file(conf_file_path + this->interface_name + conf_file_extension);
    conf_file << "interface=" << this->interface_name << std::endl;
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
    if(0 != System::call("hostapd -P " + this->getPidFile() + " " + this->getConfigurationFile()))
    {
        throw std::runtime_error("Failed to start hostapd");
    }
}

void HotSpotController::stop()
{
    std::string pid_file = this->getPidFile();
    if(0 != System::call("kill $( cat " + pid_file + ")"))
    {
        if(0 != System::call("kill -9 $( cat " + pid_file + ")"))
        {
            throw std::runtime_error("Failed to stop hostapd");
        }
    }
    std::remove(pid_file.c_str());
}

void HotSpotController::reload()
{
    if(0 != System::call("systemctl reload hostapd"))
    {
        throw std::runtime_error("Failed to stop hostapd");
    }
}
