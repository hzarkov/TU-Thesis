#include "NetworkManager.hpp"
#include "Logger.hpp"
#include "INIReader.h"
#include "DLoader.hpp"
#include "Plugin.hpp"

#include <memory>
#include <signal.h>
#include <sstream>

const std::string NM_PLUGIN_CONFIGURATION_FILE = "../config/nmPluginConfiguration.ini";

std::map<std::string,std::string> getPluginConfiguration(INIReader& reader, std::string plugin_type)
{
    std::map<std::string,std::string> configuration;
    if("ini_nm_configurator" == plugin_type)
    {   
        std::string ini_file(reader.Get(plugin_type, "file", ""));
        configuration["file"] = ini_file;
    }
    else if("ping_internet_switcher" == plugin_type)
    {

        std::string interfaces_string(reader.Get(plugin_type, "interfaces", ""));
        std::vector<std::string> interfaces;
        std::stringstream ss(interfaces_string);
        std::string item;
        while (std::getline(ss, item, ',')) 
        {
            interfaces.push_back(item);
        }
    }
    else if("routing_specializer" == plugin_type)
    {
        std::string interface_string(reader.Get(plugin_type, "interfaces", ""));

        std::stringstream ss(interface_string);
        std::string interface_name;
        while (std::getline(ss, interface_name, ',')) 
        {
            std::string specialization_destinations(reader.Get(plugin_type, interface_name, ""));
            configuration[interface_name] = specialization_destinations;
        }            
    }
    return configuration;
}

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

        std::shared_ptr<DLoader> plugin_dl;
        try
        {
            plugin_dl = loaded_plugins.at(plugin_type);
        }
        catch(std::exception& e)
        {
            plugin_dl = std::make_shared<DLoader>("../lib/lib" + plugin_type + ".so");
            loaded_plugins[plugin_type] = plugin_dl;
        }

        std::shared_ptr<Plugin> plugin = 
            plugin_dl->createInstance<Plugin>(network_manager);
        plugin_instances.push_back(plugin);

        auto configuration = getPluginConfiguration(reader, plugin_type);
        try
        {
            plugin->configure(configuration);
            plugin->exec();
        }
        catch(std::exception& e)
        {
            ErrorLogger << plugin_type << ": " << e.what() << std::endl;
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