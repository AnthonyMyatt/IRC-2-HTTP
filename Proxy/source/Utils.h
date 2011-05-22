#ifndef UTILS_H
#define UTILS_H

#include <SocketHandler.h>
#include "IrcSession.h"

#include <list>

class Utils
{
public:
    static SocketHandler& get_socket_handler();
    static bool irc_socket_log();
    static bool is_httpd_stop();
    static void httpd_stop();
    static void init(std::string fconf);
    static std::string get_irc_host();
    static std::string get_web_domain();
    static std::string get_webirc_user();
    static std::string get_webirc_pass();
    static unsigned int get_irc_port();
    static unsigned int get_server_port();
    static int get_max_clients();
    static int get_timeout();
    static std::string get_timeout_msg();
protected:
private:
    Utils(){};
    static SocketHandler m_handler;
    static bool m_httpd_stop;
    static std::string irc_host;
    static unsigned int irc_port;
    static int timeout;
    static std::string timeout_msg;
    static int max_clients;
    static unsigned int server_port;
    static std::string web_domain;
    static std::string webirc_user;
    static std::string webirc_pass;

    static std::string prepare_conf_line(std::string);

};

#endif // UTILS_H
