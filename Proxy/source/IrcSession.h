#ifndef IRCSESSION_H
#define IRCSESSION_H

#include "IrcClientListener.h"
#include "IrcClient.h"

#include <deque>


class IrcSession : public IrcClientListener
{
public:
    IrcSession(const std::string& id);
    virtual ~IrcSession();
    IrcClient& get_client();
    bool has_messages();
    std::string get_message();
    std::string get_id();
    void connect(std::string nick, std::string user, std::string name, std::string host, std::string ip, std::string device);
    void stamp();
    time_t get_stamp();


private:
    std::string m_id;
    IrcClient* m_client;
    std::deque<std::string> msgs;
    unsigned int m_seq;
    time_t m_stamp;

    virtual void on_connect();
    virtual void on_privmsg(std::string from, std::string to, std::string msg);
    virtual void on_notice(std::string from, std::string to, std::string msg);
    virtual void on_nick(std::string from, std::string new_nick);
    virtual void on_join(std::string from, std::string chan);
    virtual void on_part(std::string from, std::string chan);
    virtual void on_userinfo(std::string nick);
    virtual void on_msg(std::string from, std::string cmd, std::string msg);
    virtual void on_motd(std::string msg);



};

#endif // IRCSESSION_H
