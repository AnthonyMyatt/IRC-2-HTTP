#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>


SocketHandler Utils::m_handler;
bool Utils::m_httpd_stop = false;
std::string Utils::irc_host = "irc.tribefm.com";
unsigned int Utils::irc_port = 6667;
int Utils::timeout = 60;
std::string Utils::timeout_msg = "Client has timed out!";
int Utils::max_clients = 1000;
unsigned int Utils::server_port = 20002;
std::string Utils::web_domain = "chat.tribefm.com";
std::string Utils::webirc_user = "IRC2HTTP";
std::string Utils::webirc_pass = "pass";

/**
 *  Prepares a configuration file line for parsing.
 *  This function removes comments and returns the result string.
 *  If the resulting string is all whitespaces, it will return
 *  an empty string, otherwise it will not attempt to remove whitespaces.
 */
std::string Utils::prepare_conf_line(std::string line)
{
    unsigned int idx = line.find('#');
    if(idx != line.npos);
    {
        line = line.substr(0 , idx);
    }
    for(unsigned int i = 0 ; i < line.length() ; i++)
    {
        if(line[i] == ' ' || line[i] == '\t')
            continue;
        return(line);
    }
    return("");
}




/**
 *  Parses a configuration file and setup options.
 *  This function will read and parse a configuration file name
 *  <fconf> and reads configuration options.
 *  Current options are:
 *  Name            Type        Default
 *  * ServerPort    int         20002
 *  * MaxClients    int         1000
 *  * Timeout       int         60
 *  * QuitMsg       string      Client has timed out
 *  * IrcServer     string      irc.tribefm.com
 *  * IrcPort       int         6667
 */
void Utils::init(std::string fconf)
{
    if(fconf.empty())
        return;

    std::ifstream conf(fconf.c_str());
    std::string line;
    while(!conf.eof())
    {
        getline(conf , line);
        line = prepare_conf_line(line);
        if(line.empty())
            continue;

        std::istringstream ss(line);
        std::string key , value , dum;
        int ivalue;
        ss >> key;
        ss >> dum;
        if(key == "ServerPort")
        {
            ss >> ivalue;
            server_port = ivalue;
        }
        else if(key == "MaxClients")
        {
            ss >> ivalue;
            max_clients = ivalue;
        }
        else if(key == "Timeout")
        {
            ss >> ivalue;
            timeout = ivalue;
        }
        else if(key == "QuitMsg")
        {
            getline(ss , value);
            timeout_msg = value;
        }
        else if(key == "IrcServer")
        {
            ss >> value;
            irc_host = value;
        }
        else if(key == "IrcPort")
        {
            ss >> ivalue;
            irc_port = ivalue;
        }
        else if(key == "WebDomain")
        {
            ss >> value;
            web_domain = value;
        }
        else if(key == "WebIrcUser")
        {
            ss >> value;
            webirc_user = value;
        }
        else if(key == "WebIrcPass")
        {
            ss >> value;
            webirc_pass = value;
        }
        else
        {
            std::cerr << "Error reading " << fconf << std::endl;
            std::cerr << "Invalid Key: " << key << std::endl;
        }
    }

}

/** @brief get_socket_handler
  *
  */
SocketHandler& Utils::get_socket_handler()
{
    return(m_handler);
}

/** @brief get_irc_port
  *
  */
unsigned int Utils::get_irc_port()
{
    return(irc_port);
}

/** @brief get_irc_host
  *
  */
std::string Utils::get_irc_host()
{
    return(irc_host);
}

/** @brief get_web_domain
  *
  */
std::string Utils::get_web_domain()
{
    return(web_domain);
}

/** @brief get_webirc_user
  *
  */
std::string Utils::get_webirc_user()
{
    return(webirc_user);
}

/** @brief get_webirc_user
  *
  */
std::string Utils::get_webirc_pass()
{
    return(webirc_pass);
}

bool Utils::irc_socket_log()
{
    return(true);
}


bool Utils::is_httpd_stop()
{
    return(m_httpd_stop);
}

void Utils::httpd_stop()
{
    m_httpd_stop = true;
}

unsigned int Utils::get_server_port()
{
    return(server_port);
}

int Utils::get_max_clients()
{
    return(max_clients);
}

int Utils::get_timeout()
{
    return(timeout);
}

std::string Utils::get_timeout_msg()
{
    return(timeout_msg);
}
