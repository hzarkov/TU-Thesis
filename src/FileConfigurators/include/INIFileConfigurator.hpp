#ifndef INI_FILE_CONFIGURATOR_HPP
#define INI_FILE_CONFIGURATOR_HPP
#include "FileConfigurator.hpp"
#include "NetworkManager.hpp"
#include <string>

class INIFileConfigurator : public FileConfigurator
{
private:

public:
    INIFileConfigurator(std::shared_ptr<NetworkManager> nm);
    ~INIFileConfigurator() = default;
    void exec();
    void config(std::map<std::string, std::string> conf);
};

#endif