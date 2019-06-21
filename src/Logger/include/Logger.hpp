#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <iostream>
#include <sstream>
#define DebugLogger Logger("Debug", __FILE__, __FUNCTION__, __LINE__)
#define InformationLogger Logger("Information", __FILE__, __FUNCTION__, __LINE__)
#define WarningLogger Logger("Warning", __FILE__, __FUNCTION__, __LINE__)
#define ErrorLogger Logger("Error", __FILE__, __FUNCTION__, __LINE__)

class Logger
{
public:
    Logger(std::string name, const char* file_name, const char* function, int line);

    template<typename T>
    Logger& operator<<(const T& data)
    {
        this->ss << data;
        return *this;
    }

    Logger& operator<<(std::ostream& (*os)(std::ostream&));

private:
    std::stringstream ss;
};

#endif