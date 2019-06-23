#include "System.hpp"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
namespace System
{
    int call(std::string command)
    {
        return system(command.c_str());
    }

    std::string exec(std::string command) {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }
}
