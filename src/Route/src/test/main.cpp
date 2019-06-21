#include <chrono>
#include <thread>

#include "Logger.hpp"
#include "Route.hpp"

int main(int argc, char const *argv[])
{
    Route route(IP("8.8.8.8/32"),IP("192.168.0.1"));
    InformationLogger << "Route added " << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    return 0;
}