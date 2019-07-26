#include "ChainRule.hpp"

#include <stdexcept>

ChainRule::ChainRule(std::string rule)
:rule(rule)
{
    if(0 != system(("iptables -N " + this->rule).c_str()))
    {
        throw std::runtime_error("System execution of '" + rule + "' failed.");
    }
}

std::string ChainRule::getRule()
{
    return this->rule;
}

ChainRule::~ChainRule()
{
    system(("iptables -X " + this->rule).c_str());
}