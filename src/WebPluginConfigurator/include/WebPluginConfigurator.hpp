#ifndef WEB_PLUGIN_CONFIGURATOR_HPP
#define WEB_PLUGIN_CONFIGURATOR_HPP

#include "PluginConfigurator.hpp"
#include <string>
#include <vector>
#include <map>
#include <netinet/in.h>
#include <mutex>

class WebPluginConfigurator : public PluginConfigurator
{
public:
    WebPluginConfigurator();
    void start();
    void stop();
    ~WebPluginConfigurator();
private:
    typedef std::map<std::string,std::string> Data_t;
    typedef std::map<std::string,std::string> Header_t;
    struct Request
    {
        Header_t header;
        Data_t data;
    };

    Header_t parseHeader(std::string& request);
    Data_t parseData(std::string& request);
    Request readRequest(int client_socket);
    void sendMessage(int client_socket, std::string content_type, std::string& message);
    std::string processRequest(int client_socket, Request& request);
    size_t getPluginIdOfRequest(Request& request);
    std::string generateHTMLMessage(Request& request);

    void AccpetingThread();
    void ClientThread(int client_socket);

    int server_socket;
    struct sockaddr_in address;

    std::thread accept_clients_thread;
    bool accept_new_clients;

};
#endif