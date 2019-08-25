#include "Logger.hpp"
#include <ctime>


//std::mutex Logger::print_mutex; //ToDo: I am not sure that this is good solution to synchronize logs, but it works for now. Would be problem if need to sync logs from multiple processes.

Logger::Logger(std::string name, const char* file_name, const char* function, int line)
{
    this->ss.str("");
    std::time_t current_time = std::time(nullptr);
    this->ss << name << " [time=" << current_time << ", file=" << file_name << ", function=" << function << ", line=" << line << "]: ";
}

Logger& Logger::operator<<(std::ostream& (*os)(std::ostream&))
{
    std::lock_guard<std::mutex> print_mutex_lock(print_mutex);
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
