#include "InternetSwitcher.hpp"

InternetSwitcher::InternetSwitcher(std::shared_ptr<NetworkManager> nm)
:Plugin(nm)
{

}

void InternetSwitcher::exec()
{
    this->start();
}