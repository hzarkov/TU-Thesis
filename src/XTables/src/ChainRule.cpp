#include "ChainRule.hpp"
#include "System.hpp"
#include "Logger.hpp"

#include <stdexcept>
#include <regex>

XTables::ChainRule::ChainRule(std::string table_name, std::string chain_name, std::string rule, std::string type)
:rule(rule), table_name(table_name), chain_name(chain_name)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    if(0 != System::call("iptables -t " + table_name + " " + type + " " + chain_name + " " + rule))
    {
        throw std::runtime_error("Failed to execute '" + rule + "'.");
    }
}

XTables::ChainRule::~ChainRule()
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    try
    {
        std::regex exp("-c [[:digit:]]+ [[:digit:]]+");
        std::string res_rule = std::regex_replace( this->rule, exp, std::string("") );
        std::string rule_cmd = "iptables -t " + this->table_name + " -D " + this->chain_name + " " + res_rule;
        if( 0 != System::call(rule_cmd))
        {
            WarningLogger << "Failed to delete iptables rule using '" << rule_cmd << "'" << std::endl;
        }
    }
    catch(std::exception& e)
    {
        ErrorLogger << "Failed to remove ChainRule: " << e.what() << std::endl;
    }
}