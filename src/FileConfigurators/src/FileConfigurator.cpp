#include "FileConfigurator.hpp"

FileConfigurator::FileConfigurator(std::shared_ptr<NetworkFactory> nm)
:Plugin(nm)
{

}

void FileConfigurator::configure(Plugin::Configuration_t conf)
{
    this->file_name = conf.at("file");
    this->configureFileConfigurator(conf);
}

Plugin::Configuration_t FileConfigurator::getConfiguration()
{
    Plugin::Configuration_t result = this->getFileConfiguratorConfiguration();
    result["file"] = this->file_name;
    return result;
}