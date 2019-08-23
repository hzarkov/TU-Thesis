#include "NetworkManager.hpp"
#include "Logger.hpp"
#include "INIReader.h"
#include "DLoader.hpp"
#include "Configurator.hpp"
#include "InternetSwitcher.hpp"
#include "TrafficSpecializer.hpp"

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
            std::shared_ptr<DLoader> plugin_dl;
            try
            {
                plugin_dl = loaded_plugins.at(plugin_type);
            }
            catch(std::exception& e)
            {
                plugin_dl = std::make_shared<DLoader>("../lib/libini_nm_configurator.so");
                loaded_plugins[plugin_type] = plugin_dl;
            }

            std::shared_ptr<Configurator> conf = 
                plugin_dl->createInstance<Configurator>(network_manager, ini_file);
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
            std::shared_ptr<DLoader> plugin_dl;
            try
            {
                plugin_dl = loaded_plugins.at(plugin_type);
            }
            catch(std::exception& e)
            {
                plugin_dl = std::make_shared<DLoader>("../lib/libping_internet_switcher.so");
                loaded_plugins[plugin_type] = plugin_dl;
            }

            std::shared_ptr<InternetSwitcher> internet_switcher = 
                plugin_dl->createInstance<InternetSwitcher>(network_manager, interfaces);
            internet_switcher->start();
            plugin_instances.push_back(internet_switcher);
        }
        else if("RoutingSpecializer" == plugin_type)
        {
            std::string interface_string(reader.Get(plugin_type, "interface", ""));
            std::string destinations_string(reader.Get(plugin_type, "specialized_destinations", ""));
            DebugLogger << interface_string << std::endl;
            std::vector<std::string> spec_dests;
            std::stringstream ss(destinations_string);
            std::string item;
            while (std::getline(ss, item, ',')) 
            {
                spec_dests.push_back(item);
            }            

            std::shared_ptr<DLoader> plugin_dl;
            try
            {
                plugin_dl = loaded_plugins.at(plugin_type);
            }
            catch(std::exception& e)
            {
                plugin_dl = std::make_shared<DLoader>("../lib/librouting_specializer.so");
                loaded_plugins[plugin_type] = plugin_dl;
            }
            std::shared_ptr<TrafficSpecializer> traffic_specizlizer = 
                plugin_dl->createInstance<TrafficSpecializer>(network_manager, interface_string, spec_dests);
            plugin_instances.push_back(traffic_specizlizer);
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