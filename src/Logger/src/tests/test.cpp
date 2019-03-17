#include "Logger.hpp"

std::string print()
{

    DebugLogger << "Hello by Print" << std::endl;
    return "PRINTED";
}
int main(int argc, char const *argv[])
{
    DebugLogger << "Hello Debug Logger" << print() << std::endl;
    //InformationLogger << "Hello Information Logger" << std::endl;
    //WarningLogger << "Hello Warning Logger" << std::endl;
    return 0;
}