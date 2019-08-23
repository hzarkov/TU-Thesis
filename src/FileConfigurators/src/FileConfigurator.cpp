#include "FileConfigurator.hpp"

FileConfigurator::FileConfigurator(std::shared_ptr<NetworkManager> nm)
:Plugin(nm)
{

}

std::string FileConfigurator::getFileName()
{
    return this->file_name;
}

void FileConfigurator::configure(std::map<std::string, std::string> conf)
{
    this->file_name = conf.at("file");
    this->config(conf);
}