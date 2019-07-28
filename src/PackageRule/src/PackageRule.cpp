#include "PackageRule.hpp"
#include "System.hpp"

#include <stdexcept>
#include <regex>

PackageRule::PackageRule(std::string rule, std::string type)
:rule(rule)
{
    if(0 != System::call("iptables " + type + " " + rule))
    {
        throw std::runtime_error("Failed to execute '" + rule + "'.");
    }
}

std::string PackageRule::getRule()
{
    return this->rule;
}

PackageRule::~PackageRule()
{
    std::regex exp("-c [[:digit:]]+ [[:digit:]]+");
    std::string res_rule = std::regex_replace( this->rule, exp, "" );
    std::string rule_cmd = "iptables -D " + res_rule;
    if ( 0 != System::call(rule_cmd) )
    {
        throw std::runtime_error( "Failed to execute '" + rule_cmd + "'." );
    }
}