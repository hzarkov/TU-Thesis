#include "ElapsedTime.hpp"


void ElapsedTime::start()
{
    this->t_start = std::chrono::steady_clock::now();
}

int ElapsedTime::ready()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->t_start).count();
}
