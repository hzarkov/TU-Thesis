#include "Logger.hpp"

std::string print()
{

    DebugLogger << "Hello by Print" << std::endl;
    return "Print result";
}
int main(int argc, char const *argv[])
{
    DebugLogger << "Hello Debug Logger: " << print() << std::endl;
    //InformationLogger << "Hello Information Logger" << std::endl;
    //WarningLogger << "Hello Warning Logger" << std::endl;
    return 0;
}