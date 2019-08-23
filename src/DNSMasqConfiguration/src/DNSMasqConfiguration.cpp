#include "DNSMasqConfiguration.hpp"
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <cstdio>

DNSMasqConfiguration::DNSMasqConfiguration(std::string file_name, std::vector<Configuration_t> configuration)
:file_name(file_name), configuration(configuration)
{
    //ToDo: read current configuration of the file
    this->writeConfiguration();
}

void DNSMasqConfiguration::addConfiguration(std::string name, std::string value)
{
    this->configuration.push_back(std::make_pair(name, value));
    this->writeConfiguration();
}

void DNSMasqConfiguration::addConfiguration(std::vector<Configuration_t> configuration)
{
    this->configuration.insert(this->configuration.end(), configuration.begin(), configuration.end());
    this->writeConfiguration();
}

void DNSMasqConfiguration::replaceConfiguration(std::string name, std::string old_value, std::string new_value)
{
    auto it = std::find_if(this->configuration.begin(), this->configuration.end(), [name, old_value](Configuration_t conf){
        return name == conf.first && old_value == conf.second;
    });
    if(this->configuration.end() != it)
    {
        it->second = new_value;
    }
    else
    {
        throw std::invalid_argument("Couldn't find configuration with name '" + name + "' and '" + old_value + "' value");
    }
    this->writeConfiguration();
}

void DNSMasqConfiguration::writeConfiguration()
{
    std::lock_guard<std::mutex> file_lock(this->file_mutex);
    std::ofstream conf_file(this->file_name);
    for(auto conf : this->configuration)
    {
        conf_file << conf.first << "=" << conf.second << std::endl;
    }
}

DNSMasqConfiguration::~DNSMasqConfiguration()
{
    std::remove(file_name.c_str());
}