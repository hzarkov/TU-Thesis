#ifndef FILE_CONFIGURATOR_HPP
#define FILE_CONFIGURATOR_HPP
#include "Configurator.hpp"

#include <string>
class FileConfigurator : public Configurator
{
protected:
    std::string file_name;
public:
    virtual ~FileConfigurator() = default;
    FileConfigurator(std::shared_ptr<NetworkManager> nm, std::string file_name);
    std::string getFileName();
};

#endif