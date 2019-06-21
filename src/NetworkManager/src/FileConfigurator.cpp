#include "FileConfigurator.hpp"

FileConfigurator::FileConfigurator(std::shared_ptr<NetworkManager> nm, std::string file_name)
:Configurator(nm), file_name(file_name)
{

}

std::string FileConfigurator::getFileName()
{
    return this->file_name;
}