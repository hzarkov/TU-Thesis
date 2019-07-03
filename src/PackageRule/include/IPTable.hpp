#ifndef IP_TABLE_HPP
#define IP_TABLE_HPP

class IPTable
{
private:
public:
    virtual ~IPTable() = default;
    
    virtual std::shared_ptr<PackageRule> addRule(std::string rule, std::string ruleType="-A") = 0;
    virtual void removeRule(std::shared_ptr<PackageRule> rule) = 0;
    
    virtual std::shared_ptr<PackageRule> addRule(std::string rule, std::string ruleType="-A") = 0;
    virtual void removeRule(std::shared_ptr<PackageRule> rule) = 0;
};
#endif