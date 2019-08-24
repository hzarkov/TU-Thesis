#include "WebPluginConfigurator.hpp"

#include <signal.h>
#include <iostream>
int main()
{
    // Block stop signals.
    sigset_t sigSet;
    int sigStop;
    (void)sigemptyset(&sigSet);
    (void)sigaddset(&sigSet, SIGINT);
    (void)sigaddset(&sigSet, SIGTERM);

    if (0 != pthread_sigmask(SIG_BLOCK, &sigSet, NULL))
    {
        std::cout << "Failed to block stop signals." << std::endl;
        return EXIT_FAILURE;
    }

    WebPluginConfigurator wpc;
    wpc.start();

    // Wait for stop signal (SIGINT, SIGTERM).
    if (0 != sigwait(&sigSet, &sigStop))
    {
        std::cout << "Failed to wait stop signal." << std::endl;
        return EXIT_FAILURE;
    }

    switch (sigStop)
    {
        case SIGINT:
        case SIGTERM:
            //wpc.stop();
            break;
        default:
            std::cout << "Caught unexpected stop signal." << std::endl;
            return EXIT_FAILURE;
    }

}