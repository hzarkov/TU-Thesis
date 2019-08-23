#ifndef PACKAGE_RULE_HPP
#define PACKAGE_RULE_HPP
#include<string>
namespace XTables
{
    class Rule
    {
    public:
        Rule(std::string rule, std::string type = "-A");
        ~Rule();
    private:
       std::string rule;    
    };
};
#endif