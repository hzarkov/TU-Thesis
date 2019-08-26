#ifndef DLOADER_H
#define DLOADER_H
#include <string>
#include <memory>
#include <dlfcn.h>

class DLoader
{
public:
    DLoader(std::string library_path,std::string allocator_name = "allocator",
        std::string deallocator_name = "deallocator");
    
    template<typename T,typename... A>
    std::shared_ptr<T> createInstance(A... args)
    {

        void* allocator_function = dlsym(this->handle, this->allocator_name.c_str());

        void* deallocator_function = dlsym(this->handle, this->deallocator_name.c_str());

        if(NULL == allocator_function || NULL == deallocator_function)
        {
            throw std::runtime_error(dlerror());
        }

        typedef T *(*allocator_t)(A...);
        typedef void (*deallocator_t)(T *);

        auto alloc_func = reinterpret_cast<allocator_t>(allocator_function);

        auto dealloc_func = reinterpret_cast<deallocator_t>(deallocator_function);

        return std::shared_ptr<T>(alloc_func(args...),[dealloc_func](T* p){dealloc_func(p);});
    }

    ~DLoader();
private:
    void* handle;
    std::string allocator_name;
    std::string deallocator_name;
};

#endif