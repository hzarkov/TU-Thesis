#include "ChainRule.hpp"
#include "System.hpp"

#include <stdexcept>

ChainRule::ChainRule(std::string rule)
:rule(rule)
{
    if(0 != System::call("iptables -N " + this->rule))
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
    System::call("iptables -X " + this->rule);
}