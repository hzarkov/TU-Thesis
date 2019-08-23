#ifndef IP_SET_HPP
#define IP_SET_HPP

#include <string>

class IPSet
{
public:
    IPSet(std::string name, std::string type);
    ~IPSet(); 
private:
    std::string name;
};
#endif