#ifndef XTABLES_CHAIN_RULE_HPP
#define XTABLES_CHAIN_RULE_HPP
#include<string>
namespace XTables
{
    class ChainRule
    {
    public:
        ChainRule(std::string rule, std::string type = "-A");
        ~ChainRule();
    private:
       std::string rule;    
    };
};
#endif