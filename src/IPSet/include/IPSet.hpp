#ifndef IPSET_HPP
#define IPSET_HPP
#include <string>

class IPSet
{
public:
    IPSet(std::string name, std::string type);
    std::string getName();
    void addIP(std::string ip);
    ~IPSet();
private:
    std::string name;
};

#endif