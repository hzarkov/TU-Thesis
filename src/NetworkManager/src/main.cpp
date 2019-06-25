#include "NetworkManager.hpp"
#include "Logger.hpp"
#include "INIReader.h"
#include "DLoader.hpp"
#include "Configurator.hpp"

#include <memory>
#include <signal.h>
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
    std::map<std::string,std::shared_ptr<DLoader>> loaded_configurations;
    std::vector<std::shared_ptr<Configurator>> configuration_instances;
    for (std::set<std::string>::iterator it = sections.begin(); it != sections.end(); ++it)
    {
        std::string configuration_type = *it;
        if("INIFileConfigurator" == configuration_type)
        {
            std::string ini_file(reader.Get(configuration_type, "file", ""));
            loaded_configurations[configuration_type] = std::make_shared<DLoader>("../lib/libini_nm_configurator.so");
            std::shared_ptr<Configurator> conf = 
                loaded_configurations[configuration_type]->createInstance<Configurator>(network_manager, ini_file);
            conf->configure();
            configuration_instances.push_back(conf);
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