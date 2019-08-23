#ifndef FILE_CONFIGURATOR_HPP
#define FILE_CONFIGURATOR_HPP
#include "Plugin.hpp"

#include <string>
class FileConfigurator : public Plugin
{
protected:
    std::string file_name;
public:
    virtual ~FileConfigurator() = default;
    FileConfigurator(std::shared_ptr<NetworkManager> nm);
    void configure(std::map<std::string, std::string> conf);
    virtual void config(std::map<std::string, std::string> conf) = 0;
    std::string getFileName();
};

#endif