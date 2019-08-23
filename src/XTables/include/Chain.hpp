#ifndef CHAIN_RULE_HPP
#define CHAIN_RULE_HPP
#include "Rule.hpp"

#include <string>
#include <map>
#include <memory>
#include <mutex>

namespace XTables
{
    class Chain
    {
    public:
        typedef int RuleID;
        Chain(std::string name);
        ~Chain();
        RuleID addRules(std::string rule, std::string type = "-A");
        void removeRule(RuleID id);
        std::string getName();
    private:
       std::string name;
       std::map<RuleID, std::unique_ptr<XTables::Rule>> package_rules;
       std::mutex package_rules_mutex;
    };
};

#endif