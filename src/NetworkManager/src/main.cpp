#include "NetworkManager.hpp"
#include "Logger.hpp"
#include "INIFileConfigurator.hpp"

#include <memory>
#include <signal.h>

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

    std::this_thread::sleep_for (std::chrono::seconds(1));
    INIFileConfigurator ini_file_configurator(network_manager, "configuration.ini");
    ini_file_configurator.configure();

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