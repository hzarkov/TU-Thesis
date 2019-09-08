#ifndef IPSET_HPP
#define IPSET_HPP
#include <string>
#include <vector>

class IPSet
{
public:
    IPSet(std::string name, std::string type);
    std::string getName();
    void addIP(std::string ip);
    std::vector<std::string> getIPs();
    void clearIPs();
    ~IPSet();
private:
    std::string name;
    std::vector<std::string> ips;
};

#endif