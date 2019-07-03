#ifndef LINUX_IP_TABLE_HPP
#define LINUX_IP_TABLE_HPP
#include "IPTable.hpp"

class LinuxIPTable : public IPTable
{
private:
    std::string table_name;
    std::map<std::string, std::shared_ptr<TableChain>> chains;
public:
    LinuxIPTable(std::string table_name);
    std::shared_ptr<TableChain> addChain();
    void removeChain(std::shared_ptr<TableChain>);
};
#endif