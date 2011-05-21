#ifndef IRCCLIENT_H
#define IRCCLIENT_H

#include "IrcClientListener.h"
#include "IrcSocket.h"
#include "UserInfo.h"

class IrcClient : public virtual IrcSocketListener
{
public:
    IrcClient(std::string host , unsigned int port);
    virtual ~IrcClient();

    UserInfo get_user_info();
    bool connect(std::string nick, std::string user, std::string name, std::string host, std::string ip, std::string device);
    bool is_connected();
    void quit(std::string msg);
    void raw(std::string msg);
    void message(std::string to , std::string msg);
    void notice(std::string to, std::string msg);
    void emote(std::string to , std::string msg);
    void join(std::string chan);
    void part(std::string chan);
    void nick(std::string nick);
    void away(std::string msg);
    void add_listener(IrcClientListener* listener);
    void remove_listener(IrcClientListener* listener);
    // IrcSocketListener overrides
    void on_line(const std::string& line);
protected:
private:
    void notify_listeners();
    void parse(const std::string& line , std::string& prefix , std::string& cmd , std::string& arg);

    bool m_connected;
    // client data
    std::string m_nick;
    std::string m_user;
    std::string m_name;
    // client remote host and ip
    std::string m_host;
    std::string m_ip;

    std::string m_device;

    // irc server host and port
    std::string m_server_host;
    unsigned int m_server_port;
    std::list<IrcClientListener*> m_listeners;
    IrcSocket sock;
};

#endif // IRCCLIENT_H
