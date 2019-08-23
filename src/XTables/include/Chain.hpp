#ifndef XTABLES_CHAIN_HPP
#define XTABLES_CHAIN_HPP
#include "ChainRule.hpp"

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
        Chain(std::string name, std::string table_name, bool should_create = true);
        ~Chain();
        RuleID addRule(std::string rule, std::string type = "-A");
        void removeRule(RuleID id);
        std::string getName();
    private:
        bool created;
        std::string table_name;
        std::string name;
        std::map<RuleID, std::unique_ptr<XTables::ChainRule>> package_rules;
        std::mutex package_rules_mutex;
    };
};

#endif