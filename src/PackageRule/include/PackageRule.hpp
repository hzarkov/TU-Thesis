#ifndef PACKAGE_RULE_HPP
#define PACKAGE_RULE_HPP
#include<string>
class PackageRule
{
public:
    PackageRule(std::string rule, std::string type = "-A");
    ~PackageRule();
    std::string getRule();
private:
   std::string rule;    
};
#endif