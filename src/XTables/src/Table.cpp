#include "Table.hpp"
#include <algorithm>
#include <vector>
#include "Logger.hpp"

const std::vector<std::string> main_chains = {"OUTPUT","FORWARD","INPUT","PREROUTING","POSTROUTING"};

XTables::Table::Table(std::string name)
:name(name)
{
}

std::shared_ptr<XTables::Chain> XTables::Table::getChain(std::string chain_name)
{
    std::shared_ptr<XTables::Chain> result;
    try
    {
        result = this->chains.at(chain_name).lock();
    }
    catch(std::exception& e)
    {
        bool should_create = true;
        auto it = std::find(main_chains.begin(), main_chains.end(), chain_name);
        if(main_chains.end() != it)
        {
            should_create = false;
        }
        result = std::make_shared<XTables::Chain>(chain_name, this->name, should_create);
        this->chains[chain_name] = result;
    }
    return result;
}