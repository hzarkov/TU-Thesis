#ifndef IP_HPP
#define IP_HPP

#include<string>

class IP
{
private:
    std::string ip;
public:
    IP(std::string value="");
    void set(std::string value);
    std::string toString();
    bool isEmpty();

    friend std::string operator+(std::string base, IP ip);
    friend std::ostream& operator<<(std::ostream& os, IP ip);  
};

#endif