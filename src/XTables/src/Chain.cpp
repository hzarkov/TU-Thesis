#include "Chain.hpp"
#include "System.hpp"
#include "Logger.hpp"

#include <stdexcept>

XTables::Chain::Chain(std::string name, std::string table_name, bool should_create)
:name(name), table_name(table_name), created(should_create)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    if(true == should_create && 0 != System::call("iptables -t " + table_name + " -N " + this->name))
    {
        throw std::runtime_error("Creating '" + name + "' xtable chain failed.");
    }
}

XTables::Chain::RuleID XTables::Chain::addRule(std::string rule, std::string type)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    std::lock_guard<std::mutex> package_rules_mutex_lock(this->package_rules_mutex);
    static XTables::Chain::RuleID rule_id = 0;
    this->package_rules[rule_id] = std::make_unique<ChainRule>(this->table_name, this->name, rule, type);
    return rule_id++;
}

void XTables::Chain::removeRule(XTables::Chain::RuleID id)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    std::lock_guard<std::mutex> package_rules_mutex_lock(this->package_rules_mutex);
    this->package_rules.erase(id);
}

std::string XTables::Chain::getName()
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    return this->name;
}

XTables::Chain::~Chain()
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    std::lock_guard<std::mutex> package_rules_mutex_lock(this->package_rules_mutex);
    this->package_rules.clear();
    if(true == created)
    {
        std::string cmd = "iptables -t " + this->table_name + " -X " + this->name;
        if( 0 != System::call(cmd))
        {
            WarningLogger << "Failed to delete chain using '" << cmd << "'" << std::endl;
        }
    }
}