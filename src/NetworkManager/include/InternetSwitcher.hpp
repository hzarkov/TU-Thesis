#ifndef INTERNET_SWITCHER_HPP
#define INTERNET_SWITCHER_HPP

#include "Plugin.hpp"

class InternetSwitcher : public Plugin
{
public:
    InternetSwitcher(std::shared_ptr<NetworkManager> nm);
    virtual ~InternetSwitcher() = default;
    virtual void start() = 0;
    void exec();
};
#endif