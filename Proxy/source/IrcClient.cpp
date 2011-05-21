#include "IrcClient.h"
#include "Utils.h"
#include <iostream>
#include <sstream>

/** @brief IrcClient
  *
  */
IrcClient::IrcClient(std::string host, unsigned int port)
    : m_server_host(host) , m_server_port(port) , sock(*this)
{

}


IrcClient::~IrcClient()
{
    //dtor
}


UserInfo IrcClient::get_user_info()
{
    UserInfo ui;
    ui.name = m_name;
    ui.nick = m_nick;
    ui.user = m_user;
    ui.host = m_host;
    ui.device = m_device;
    return(ui);
}


bool IrcClient::connect(std::string nick, std::string user, std::string name, std::string host, std::string ip, std::string device)
{
    m_nick = nick;
    m_name = name;
    m_user = user;
    m_host = host;
    m_ip = ip;
    m_device = device;

    sock.set_nick(nick);
    sock.set_name(name);
    sock.set_peer_host(host);
    sock.set_peer_ip(ip);

    sock.Open(m_server_host , m_server_port);
    Utils::get_socket_handler().Add(&sock);

    return(true);
}

/** @brief notify_listeners
  *
  */
void IrcClient::notify_listeners()
{

}

/** @brief remove_listener
  *
  */
void IrcClient::remove_listener(IrcClientListener* listener)
{
    m_listeners.remove(listener);
}

/** @brief add_listener
  *
  */
void IrcClient::add_listener(IrcClientListener* listener)
{
    m_listeners.push_back(listener);
}


void IrcClient::quit(std::string msg)
{
    std::string str;
    std::stringstream ss(str);
    ss << "QUIT :" << msg;
    raw(ss.str());
}


/** @brief message
  *
  */
void IrcClient::message(std::string to, std::string msg)
{
    std::string str;
    std::stringstream ss(str);
    ss << "PRIVMSG " << to << " :" << msg;
    raw(ss.str());
}

/** @brief notice
  *
  */
void IrcClient::notice(std::string to, std::string msg)
{
    std::string str;
    std::stringstream ss(str);
    ss << "NOTICE " << to << " :" << msg;
    raw(ss.str());
}

void IrcClient::emote(std::string to , std::string msg)
{
    std::string str;
    std::stringstream ss(str);
    char c = 1;
    ss << c << "ACTION " << msg << c;
    message(to , ss.str());
}

void IrcClient::raw(std::string msg)
{
    sock.Send(msg + "\n");
}

/** @brief join
  *
  */
void IrcClient::join(std::string chan)
{
    raw("JOIN " + chan);
}

/** @brief part
  *
  */
void IrcClient::part(std::string chan)
{
    raw("PART " + chan);
}

/** @brief nick
  *
  */
void IrcClient::nick(std::string nick)
{
    raw("NICK " + nick);
}


void IrcClient::away(std::string msg)
{
    raw("AWAY :" + msg);
}


/** @brief is_connected
  *
  */
bool IrcClient::is_connected()
{
    return(sock.IsConnected());
}

void IrcClient::on_line(const std::string& line)
{
    std::string prefix, cmd, arg;
    parse(line, prefix, cmd, arg);

    if(cmd == "PING")
    {
        std::string cmd = "PONG ";
        cmd += arg;
        cmd += "\n";
        sock.Send(cmd);
        return;
    }

    std::list<IrcClientListener*>::iterator it;
    for(it = m_listeners.begin() ; it != m_listeners.end() ; it++)
        (*it)->on_msg(prefix , cmd , arg);
}

void IrcClient::parse(const std::string& line , std::string& prefix , std::string& cmd , std::string& arg)
{
    unsigned int st=0,end=0;
    if(line.size() < 1)
        return;
    if(line[0] == ':')      // prefix present
    {
        st = 1;
        end = line.find(' ');
        prefix = line.substr(st, end-st);
        st = end+1;
        // check if prefix is <nick>!<user> and extract nick
        if((end = prefix.find('!')) != prefix.npos)
        {
            prefix = prefix.substr(0 , end);
        }
    }
    end = line.find(' ' , st);
    cmd = line.substr(st, end-st);
    if(end < line.size() - 1)    // params present
    {
        st = end+1;
        arg = line.substr(st, line.npos);
    }
}
