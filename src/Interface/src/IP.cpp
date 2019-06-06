#include "IP.hpp"

IP::IP(std::string value)
{
    this->set(value);
}

void IP::set(std::string value)
{
    //Add check for the ip value;
    this->ip = value;
}

std::string IP::toString()
{
    return this->ip;
}

bool IP::isEmpty()
{
    return this->ip.empty();
}

std::string operator+(std::string base, IP ip)
{
    return base+ip.toString();
}

std::ostream& operator<<(std::ostream& os, IP ip)
{
    os << ip.toString();
    return os;
} 