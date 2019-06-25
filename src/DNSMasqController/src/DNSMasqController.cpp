#include "DNSMasqController.hpp"
#include "System.hpp"

DNSMasqController::DNSMasqController()
{
    System::call("systemctl start dnsmasq");
}

DNSMasqController::~DNSMasqController()
{
    System::call("systemctl stop dnsmasq");
}

void DNSMasqController::reloadConfiguration()
{
    System::call("systemctl reload dnsmasq");
}
