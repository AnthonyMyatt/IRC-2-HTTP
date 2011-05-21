#ifndef IRCSOCKET_H
#define IRCSOCKET_H

#include <TcpSocket.h>


#include <fstream>

class IrcSocketListener
{
public:
    virtual void on_line(const std::string& line) = 0;
};


class IrcSocket : public TcpSocket
{
public:
    IrcSocket(IrcSocketListener& listener);
    virtual ~IrcSocket();
    void set_nick(std::string nick);
    void set_name(std::string name);
    void set_peer_host(std::string host);
    void set_peer_ip(std::string ip);
    void OnConnect();
    void OnLine(const std::string& line);
    void Send(const std::string& str , int f=0);
private:
    IrcSocketListener& m_listener;
    std::ofstream m_logfile;
    std::string m_nick;
    std::string m_name;
    std::string m_peer_host;
    std::string m_peer_ip;
};

#endif // IRCSOCKET_H
