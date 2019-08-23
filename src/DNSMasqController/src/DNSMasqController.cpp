#include "DNSMasqController.hpp"
#include "System.hpp"


const std::string dhcp_configuration_path = "/etc/dnsmasq.d/";
const std::string dhcp_file_extension = ".conf";

DNSMasqController::DNSMasqController()
{
    std::lock_guard<std::mutex> dnsmasq_lock(dnsmasq_mutex);
    System::call("systemctl start dnsmasq");
}

DNSMasqController::~DNSMasqController()
{
    std::lock_guard<std::mutex> dnsmasq_lock(dnsmasq_mutex);
    System::call("systemctl stop dnsmasq");
}

void DNSMasqController::reloadConfiguration()
{
    std::lock_guard<std::mutex> dnsmasq_lock(dnsmasq_mutex);
    System::call("systemctl reload dnsmasq");
}

std::shared_ptr<DNSMasqConfiguration> DNSMasqController::addConfiguration(std::string name, std::vector<DNSMasqConfiguration::Configuration_t> conf)
{
    std::lock_guard<std::mutex> dnsmasq_lock(dnsmasq_mutex);
    std::shared_ptr<DNSMasqConfiguration> result;
    try
    {
        result = this->configurations.at(name).lock();
        result->addConfiguration(conf);
    }
    catch(std::exception& e)
    {
        std::string file_name = dhcp_configuration_path + name + dhcp_file_extension;
        result = std::make_shared<DNSMasqConfiguration>(file_name, conf);
        this->configurations[name] = result;
    }
    return result;
}
