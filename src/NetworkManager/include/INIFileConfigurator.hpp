#ifndef INI_FILE_CONFIGURATOR_HPP
#define INI_FILE_CONFIGURATOR_HPP
#include "FileConfigurator.hpp"
#include "NetworkManager.hpp"
#include <string>

class INIFileConfigurator : public FileConfigurator
{
private:

public:
    INIFileConfigurator(std::shared_ptr<NetworkManager> nm, std::string file_name);
    ~INIFileConfigurator() = default;
    void configure();
};

extern "C"
{
    Configurator* allocator(std::shared_ptr<NetworkManager> nm, std::string file_name);
    void deallocator(Configurator* p);
}
#endif