#ifndef XTABLES_CHAIN_RULE_HPP
#define XTABLES_CHAIN_RULE_HPP
#include<string>
namespace XTables
{
    class ChainRule
    {
    public:
        ChainRule(std::string table_name, std::string chain_name, std::string rule, std::string type = "-A");
        ~ChainRule();
    private:
        std::string table_name;
        std::string chain_name;
        std::string rule;  
    };
};
#endif