#include "System.hpp"
int System::call(std::string command)
{
    return system(command.c_str());
}