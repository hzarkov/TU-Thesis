#ifndef DNSMASQ_CONFIGURATION_HPP
#define DNSMASQ_CONFIGURATION_HPP
#include <string>
#include <utility>
#include <vector>

#include <mutex>
class DNSMasqConfiguration
{
public:
    typedef std::pair<std::string, std::string> Configuration_t;

    DNSMasqConfiguration(std::string file_name, std::vector<Configuration_t> configuration);
    void addConfiguration(std::string name, std::string value);    
    void addConfiguration(std::vector<Configuration_t> configuration);
    void replaceConfiguration(std::string name, std::string odl_value, std::string new_value);
    ~DNSMasqConfiguration();
private:
    void writeConfiguration();
    
    std::string file_name;
    std::vector<std::pair<std::string, std::string>> configuration;
    std::mutex file_mutex;    
};
#endif