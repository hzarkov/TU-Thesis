#include "Chain.hpp"
#include "System.hpp"

#include <stdexcept>

XTables::Chain::Chain(std::string name)
:name(name)
{
    if(0 != System::call("iptables -N " + this->name))
    {
        throw std::runtime_error("Creating '" + name + "' xtable chain failed.");
    }
}

XTables::Chain::RuleID XTables::Chain::addRules(std::string rule, std::string type)
{
    std::lock_guard<std::mutex> package_rules_mutex_lock(this->package_rules_mutex);
    static XTables::Chain::RuleID rule_id = 0;
    this->package_rules[rule_id] = std::make_unique<ChainRule>(rule, type);
    return rule_id++;
}

void XTables::Chain::removeRule(XTables::Chain::RuleID id)
{
    std::lock_guard<std::mutex> package_rules_mutex_lock(this->package_rules_mutex);
    this->package_rules.erase(id);
}

std::string XTables::Chain::getName()
{
    return this->name;
}

XTables::Chain::~Chain()
{
    std::lock_guard<std::mutex> package_rules_mutex_lock(this->package_rules_mutex);
    this->package_rules.clear();
    System::call("iptables -X " + this->name);
}