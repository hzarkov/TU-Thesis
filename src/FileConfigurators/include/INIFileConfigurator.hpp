#ifndef INI_FILE_CONFIGURATOR_HPP
#define INI_FILE_CONFIGURATOR_HPP
#include "FileConfigurator.hpp"
#include <string>

class INIFileConfigurator : public FileConfigurator
{
private:

public:
    INIFileConfigurator(std::shared_ptr<NetworkFactory> nm);
    ~INIFileConfigurator() = default;
    void exec();
    void configureFileConfigurator(Configuration_t conf);
    Configuration_t getFileConfiguratorConfiguration();
};

#endif