#ifndef XTABLES_TABLE_HPP
#define XTABLES_TABLE_HPP

#include "Chain.hpp"

#include <map>
#include <string>
#include <memory>

namespace XTables
{
    class Table
    {
    public:
        Table(std::string name);
        std::shared_ptr<XTables::Chain> getChain(std::string name);
        ~Table() = default;
    private:
        std::string name;
        std::map<std::string, std::weak_ptr<XTables::Chain>> chains;
    };
};

#endif      
