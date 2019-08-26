#include "WebPluginConfigurator.hpp"

#include "Logger.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <stdexcept>
#include <sstream>
#include <regex>
#include <fstream>

constexpr int PORT = 80;
const std::string CSS_FILE = "../config/UI/design.css";
const std::string HTML_FILE = "../config/UI/index.html";

WebPluginConfigurator::WebPluginConfigurator()
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    DebugLogger << "Creating socket" << std::endl;
    if ((this->server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        throw std::runtime_error("Failed to open socket.");
    }

    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons( PORT );
    
    memset(this->address.sin_zero, '\0', sizeof this->address.sin_zero);
    
    DebugLogger << "Binding..." << std::endl;
    if (bind(this->server_socket, (struct sockaddr *)&this->address, sizeof(this->address))<0)
    {
        perror("In bind");
        close(this->server_socket);
        throw std::runtime_error("Failed to create bind.");
    }

    DebugLogger << "Adding Listen" << std::endl;
    if (listen(this->server_socket, 10) < 0)
    {
        perror("In listen");
        close(this->server_socket);
        throw std::runtime_error("Failed to create listen.");
    }
    DebugLogger << "Initialized" << std::endl;
}

void WebPluginConfigurator::start()
{    
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    this->accept_new_clients = true;
    this->accept_clients_thread = std::thread(&WebPluginConfigurator::AccpetingThread, this);
}

void WebPluginConfigurator::stop()
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    //might need to add mutex to combine both functions
    //this->accept_new_clients = false;
    //this->accept_clients_thread.join();
}

void WebPluginConfigurator::AccpetingThread()
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    DebugLogger << "Started Accepting Thread" << std::endl;
    while(true == accept_new_clients)
    {
        DebugLogger << "Waiting for client" << std::endl;
        int addrlen = sizeof(this->address);
        int client_socket = accept(this->server_socket, (struct sockaddr *)&this->address, (socklen_t*)&addrlen);
        if (0 > client_socket)
        {
            perror("In accept");
            continue;
        }
        DebugLogger << "Client accepted." << std::endl;
        std::thread(&WebPluginConfigurator::ClientThread, this, client_socket).detach();
    }
}

WebPluginConfigurator::Header_t WebPluginConfigurator::parseHeader(std::string& request)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    WebPluginConfigurator::Header_t result;
    std::stringstream ss(request);
    std::string row;
    while (std::getline(ss, row, '\n')) 
    {
        row.pop_back();
        auto found_points = row.find(":");
        if(std::string::npos != found_points)
        {
            std::string key = row.substr(0, found_points);
            std::string value = row.substr(found_points+2);
            result[key] = value;
        }
        else
        {
            if(row.find("HTTP"))
            {
                result["Type"] = row;
            }
            else
            {
                WarningLogger << "Failed to header parse '" << row << "'" << std::endl;
            }
        }
    }
    return result;
}

WebPluginConfigurator::Data_t WebPluginConfigurator::parseData(std::string& request)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    WebPluginConfigurator::Data_t result;
    std::stringstream ss(request);
    std::string row;
    while (std::getline(ss, row, '\n')) 
    {
        row.pop_back();
        auto found_points = row.find("=");
        if(std::string::npos != found_points)
        {
            std::string key = row.substr(0, found_points);
            std::string value = row.substr(found_points+1);
            result[key] = value;
        }
        else
        {
            WarningLogger << "Failed to data parse '" << row << "'" << std::endl;
        }
    }
    return result;
}
WebPluginConfigurator::Request WebPluginConfigurator::readRequest(int client_socket)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    WebPluginConfigurator::Request result;
    std::string request = "";
    //Read Header
    size_t header_end;
    do
    {
        char buffer[255] = {0};
        read( client_socket , buffer, 255);
        request += std::string(buffer);
    }while(std::string::npos == (header_end = request.find("\r\n\r\n")));

    //Parse Header
    if(std::string::npos != header_end)
    {
        std::string header = request.substr(0, header_end);
        result.header = this->parseHeader(header);
        try
        {
            int content_length = std::stoi(result.header.at("Content-Length"));
            try
            {
                request = request.substr(header_end+4);
            }
            catch(std::out_of_range& e)
            {
                request = "";
            }
            int content_to_read = content_length - request.length();
            if( 0 < content_to_read )
            {
                char buffer[content_to_read] = {0};
                read( client_socket , buffer, content_to_read);
                request += std::string(buffer);
            }
            else
            {
                DebugLogger << "There not more content to read: content_to_read=" << content_to_read << std::endl;
            }
            result.data = this->parseData(request);
        }
        catch(std::out_of_range& e)
        {
            DebugLogger << "There is no Content-Length: " << e.what() << std::endl;
        }
        catch(std::invalid_argument& e)
        {
            ErrorLogger << "Content-Length has bad value: " << result.header.at("Content-Length") << ". " << e.what() << std::endl;
        }
    }
    return result;
}

void WebPluginConfigurator::sendMessage(int client_socket, std::string content_type, std::string& message)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    std::string string_message = "\r\nHTTP/1.1 \r\nContent-Type:" + content_type + "; \r\n\r\n" + message + "\r\n\r\n";
    const char* c_message = string_message.c_str();
    write(client_socket , c_message, strlen(c_message));
}

size_t WebPluginConfigurator::getPluginIdOfRequest(WebPluginConfigurator::Request& request)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    std::string type = request.header["Type"];
    size_t plugin_field_pos = type.find("plugin=");
    if(std::string::npos != plugin_field_pos)
    {
        int plugin_id;
        std::string plugin_id_s = type.substr(plugin_field_pos+7,type.find(' ', plugin_field_pos+7));
        plugin_field_pos = std::stoi(plugin_id_s);
    }
    return plugin_field_pos;
}

std::string WebPluginConfigurator::generateHTMLMessage(WebPluginConfigurator::Request& request)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;

    std::ifstream css_file(CSS_FILE);
    std::string css_string((std::istreambuf_iterator<char>(css_file)), std::istreambuf_iterator<char>());

    std::ifstream html_file(HTML_FILE);
    std::string html_string((std::istreambuf_iterator<char>(html_file)), std::istreambuf_iterator<char>());

    html_string = std::regex_replace(html_string, std::regex("\\$CSS"), css_string);
/*    std::string result = "<html> \
    <head> \
        <style>\
            " + cssFile + "\
        </style>\
    </head> \
    <body>";*/
    std::string menu = "<ul class=\"center\">";
    for(auto plugin: this->plugins)
    {
        std::string plugin_id_s = std::to_string(plugin.first);
        std::string plugin_name = plugin_id_s;
        menu += "<a href=\"/?plugin=" + plugin_id_s + "\"><li>Plugin " + plugin_name + "</li></a>";
    }
    menu += "</ul>";
    html_string = std::regex_replace(html_string, std::regex("\\$MENU"), menu);
    
    std::string configuration = "";
    try
    {
        int plugin_id = this->getPluginIdOfRequest(request);
        if(std::string::npos != plugin_id)
        {
            std::shared_ptr<Plugin> plugin = this->plugins.at(plugin_id);
            Plugin::Configuration_t plugin_conf = plugin->getConfiguration();
            std::string plugin_name = "Plugin " + std::to_string(plugin_id);
            html_string = std::regex_replace(html_string, std::regex("\\$PLUGIN_NAME"), plugin_name);
            configuration += "<form enctype=\"text/plain\" action=\"/?plugin=" + std::to_string(plugin_id) + "\" method=\"POST\">";
            for(auto conf : plugin_conf)
            {
                configuration += "<div class=\"conf\"><label>" + conf.first + ": </label><input type=\"text\" name=\"" + conf.first + "\" value=\"" + conf.second + "\"></div>";
            }
            configuration += "<input type=\"submit\" value=\"Update\"/>";
            configuration += "</form>";
        }
    }
    catch(std::out_of_range& e)
    {

    }
    html_string = std::regex_replace(html_string, std::regex("\\$CONFIGURATION"), configuration);
    
    //result += "\t</body>\n</html>";
    return html_string;
}

std::string WebPluginConfigurator::processRequest(int client_socket, WebPluginConfigurator::Request& request)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    std::string type = request.header["Type"];
    std::string request_type = type.substr(0,type.find(' '));
    
    if("GET" == request_type)
    {
        /*std::ifstream t("index.html");
        std::string indexHTML((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        this->sendMessage(client_socket, indexHTML);*/
    }
    else if("POST" == request_type)
    {
        int plugin_id;
        try
        {
            plugin_id = this->getPluginIdOfRequest(request);
            if(std::string::npos != plugin_id)
            {
                std::shared_ptr<Plugin> plugin = this->plugins.at(plugin_id);
                plugin->configure(request.data);
            }
        }
        catch(std::invalid_argument& e)
        {
            ErrorLogger << "Incorrect plugin id string. Someone is doing something nasty!" << std::endl;
            //this->sendMessage(client_socket, "<p>" + e.what() + "<p>")
        }
        catch(std::out_of_range& e)
        {
            ErrorLogger << "Incorrect plugin id(" << plugin_id << "). Someone is doing something nasty!" << std::endl;
        }
        catch(std::exception& e)
        {
            ErrorLogger << "Failed to configure plugin " << plugin_id << " because of '" << e.what() << "'." << std::endl;
        }
    }
    return this->generateHTMLMessage(request);
}

void WebPluginConfigurator::ClientThread(int client_socket)
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;
    DebugLogger << "Reading client request." << std::endl;
    WebPluginConfigurator::Request request = this->readRequest(client_socket);

    response = this->processRequest(client_socket, request);

    this->sendMessage(client_socket, "text/html", response);
    close(client_socket);
}

WebPluginConfigurator::~WebPluginConfigurator()
{
    DebugLogger << __PRETTY_FUNCTION__ << std::endl;

    close(this->server_socket);
    //might need to add mutex to combine both functions
    this->accept_new_clients = false;
    this->accept_clients_thread.join();
}