#include "Logger.hpp"
#include <ctime>

Logger::Logger(std::string name, const char* file_name, const char* function, int line)
{
    this->ss.str("");
    std::time_t current_time = std::time(nullptr);
    this->ss << name << " [time=" << current_time << ", file=" << file_name << ", function=" << function << ", line=" << line << "]: ";
}

Logger& Logger::operator<<(std::ostream& (*os)(std::ostream&))
{
    try
    {
        std::cout << this->ss.str() << os;
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return *this;
}
