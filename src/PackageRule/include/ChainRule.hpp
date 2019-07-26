#ifndef CHAIN_RULE_HPP
#define CHAIN_RULE_HPP
#include<string>
class ChainRule
{
public:
    ChainRule(std::string rule);
    ~ChainRule();
    std::string getRule();
private:
   std::string rule;
};
#endif