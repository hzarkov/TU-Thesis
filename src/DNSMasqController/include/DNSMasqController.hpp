#ifndef DNSMASQ_CONTROLLER_HPP
#define DNSMASQ_CONTROLLER_HPP

#include "DNSMasqConfiguration.hpp"
#include <vector>
#include <map>
#include <memory>
#include <mutex>

class DNSMasqController
{
public:
    DNSMasqController();
    ~DNSMasqController();
    void reloadConfiguration();
    std::shared_ptr<DNSMasqConfiguration> addConfiguration(std::string name, std::vector<DNSMasqConfiguration::Configuration_t> configuration = std::vector<DNSMasqConfiguration::Configuration_t>());
private:
    std::mutex dnsmasq_mutex;
    std::map<std::string, std::weak_ptr<DNSMasqConfiguration>> configurations;
};
#endif