#include "ChainRule.hpp"
#include "System.hpp"

#include <stdexcept>
#include <regex>

XTables::ChainRule::ChainRule(std::string rule, std::string type)
:rule(rule)
{
    if(0 != System::call("iptables " + type + " " + rule))
    {
        throw std::runtime_error("Failed to execute '" + rule + "'.");
    }
}

XTables::ChainRule::~ChainRule()
{
    std::regex exp("-c [[:digit:]]+ [[:digit:]]+");
    std::string res_rule = std::regex_replace( this->rule, exp, "" );
    std::string rule_cmd = "iptables -D " + res_rule;
    System::call(rule_cmd);
}