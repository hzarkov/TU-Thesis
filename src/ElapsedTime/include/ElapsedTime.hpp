#ifndef ELAPSED_TIME_HPP
#define ELAPSED_TIME_HPP
#include <chrono>

class ElapsedTime
{
public:
    void start();
    int ready();
private:
    std::chrono::time_point<std::chrono::steady_clock> t_start;
};
#endif