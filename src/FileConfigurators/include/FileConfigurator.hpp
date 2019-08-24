#ifndef FILE_CONFIGURATOR_HPP
#define FILE_CONFIGURATOR_HPP
#include "Plugin.hpp"

#include <string>
class FileConfigurator : public Plugin
{
public:
    virtual ~FileConfigurator() = default;
    FileConfigurator(std::shared_ptr<NetworkFactory> nm);
    void configure(Configuration_t conf);
    virtual void configureFileConfigurator(Configuration_t conf) = 0;
    virtual Configuration_t getFileConfiguratorConfiguration() = 0;
    Configuration_t getConfiguration();
protected:
    std::string file_name;
};

#endif