#include "DLoader.hpp"

#include "Logger.hpp"

DLoader::DLoader(std::string library_path, std::string allocator_name, std::string deallocator_name)
:allocator_name(allocator_name), deallocator_name(deallocator_name)
{
    this->handle = dlopen(library_path.c_str(), RTLD_NOW | RTLD_LAZY);
    if(NULL == this->handle)
    {
        throw std::runtime_error(dlerror());
    }
}

DLoader::~DLoader()
{
    if (dlclose(this->handle)) {
        ErrorLogger << "dlclose error " << dlerror() << std::endl;
        return;
    }

}