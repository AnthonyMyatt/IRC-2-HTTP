#include "Utils.h"
#include "IrcSocket.h"

#include <iostream>

IrcSocket::IrcSocket(IrcSocketListener& listener)
    : TcpSocket(Utils::get_socket_handler()) , m_listener(listener)
{
    SetLineProtocol();
    if(Utils::irc_socket_log())
    {
        m_logfile.open("irc.log");
    }
}

IrcSocket::~IrcSocket()
{
    //dtor
}


void IrcSocket::Send(const std::string& str , int f)
{
    TcpSocket::Send(str , f);
    if(Utils::irc_socket_log())
    {
        m_logfile << ">>>" << str << std::endl;
        m_logfile.flush();
    }
}


void IrcSocket::set_nick(std::string nick)
{
    m_nick = nick;
}

void IrcSocket::set_name(std::string name)
{
    m_name = name;
}

void IrcSocket::set_peer_host(std::string host)
{
    m_peer_host = host;
}

void IrcSocket::set_peer_ip(std::string ip)
{
    m_peer_ip = ip;
}

void IrcSocket::OnConnect()
{
    // m_nick, m_name, m_peer_host and m_peer_ip are already set. 

    std::string cmd = "WEBIRC ais22b cgiirc ";
    cmd += m_peer_host;
    cmd += " ";
    cmd += m_peer_ip;
    cmd += "\n";
    Send(cmd);
    cmd = "NICK ";
    cmd += m_nick;
    cmd += "\n";
    Send(cmd);
    cmd = "USER ";
    cmd += m_nick;
    cmd += " 0 * :";
    cmd += m_name;
    cmd += "\n";
    Send(cmd);
}


/** @brief OnLine
  *
  */
void IrcSocket::OnLine(const std::string& line)
{
    if(Utils::irc_socket_log())
    {
        m_logfile << "<<<" << line << std::endl;
        m_logfile.flush();
    }
    m_listener.on_line(line);
}

