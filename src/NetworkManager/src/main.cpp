#include "NetworkManager.hpp"
#include "Logger.hpp"
#include "INIReader.h"
#include "DLoader.hpp"
#include "Configurator.hpp"
#include "InternetSwitcher.hpp"

#include <memory>
#include <signal.h>
#include <sstream>

const std::string NM_PLUGIN_CONFIGURATION_FILE = "../config/nmPluginConfiguration.ini";
int main(int argc, char const *argv[])
{
    // Block stop signals.
    sigset_t sigSet;
    int sigStop;
    (void)sigemptyset(&sigSet);
    (void)sigaddset(&sigSet, SIGINT);
    (void)sigaddset(&sigSet, SIGTERM);

    if (0 != pthread_sigmask(SIG_BLOCK, &sigSet, NULL))
    {
        ErrorLogger << "Failed to block stop signals." << std::endl;
        return EXIT_FAILURE;
    }

    std::shared_ptr<NetworkManager> network_manager = 
        std::make_shared<NetworkManager>();
    network_manager->start();

    INIReader reader(NM_PLUGIN_CONFIGURATION_FILE);

    if (reader.ParseError() < 0) {
        throw std::runtime_error("Can't load " + NM_PLUGIN_CONFIGURATION_FILE);
    }
    std::set<std::string> sections = reader.Sections();
    std::map<std::string,std::shared_ptr<DLoader>> loaded_plugins;
    std::vector<std::shared_ptr<Plugin>> plugin_instances;
    for (std::set<std::string>::iterator it = sections.begin(); it != sections.end(); ++it)
    {
        std::string plugin_type = *it;
        if("INIFileConfigurator" == plugin_type)
        {
            std::string ini_file(reader.Get(plugin_type, "file", ""));
            loaded_plugins[plugin_type] = std::make_shared<DLoader>("../lib/libini_nm_configurator.so");
            std::shared_ptr<Configurator> conf = 
                loaded_plugins[plugin_type]->createInstance<Configurator>(network_manager, ini_file);
            conf->configure();
            plugin_instances.push_back(conf);
        }
        else if("PingInternetSwitcher" == plugin_type)
        {
            std::string interfaces_string(reader.Get(plugin_type, "interfaces", ""));
            std::vector<std::string> interfaces;
            std::stringstream ss(interfaces_string);
            std::string item;
            while (std::getline(ss, item, ',')) 
            {
                interfaces.push_back(item);
            }
            loaded_plugins[plugin_type] = std::make_shared<DLoader>("../lib/libping_internet_switcher.so");
            std::shared_ptr<InternetSwitcher> internet_switcher = 
                loaded_plugins[plugin_type]->createInstance<InternetSwitcher>(network_manager, interfaces);
            internet_switcher->start();
            plugin_instances.push_back(internet_switcher);
        }
    }
    // Wait for stop signal (SIGINT, SIGTERM).
    if (0 != sigwait(&sigSet, &sigStop))
    {
        ErrorLogger << "Failed to wait stop signal." << std::endl;
        return EXIT_FAILURE;
    }

    switch (sigStop)
    {
        case SIGINT:
        case SIGTERM:
            network_manager->stop();
            break;
        default:
            ErrorLogger << "Caught unexpected stop signal." << std::endl;
            return EXIT_FAILURE;
    }

    return 0;
}