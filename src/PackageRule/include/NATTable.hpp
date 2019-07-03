#ifndef NAT_TABLE_HPP
#define NAT_TABLE_HPP
#include "IPTable.hpp"

class NATTable : public IPTable
{
private:
    const std::string name = "NAT";
public:

    std::shared_ptr<PackageRule> addRule(std::string rule, std::string ruleType="-A") = 0;
    void removeRule(std::shared_ptr<PackageRule> rule) = 0;
};
#endif