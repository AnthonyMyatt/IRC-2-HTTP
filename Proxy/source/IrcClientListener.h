#ifndef IRCCLIENTLISTENER_H
#define IRCCLIENTLISTENER_H

#include <string>


class IrcClientListener
{
public:
    virtual void on_connect() {};
    virtual void on_privmsg(std::string from, std::string to, std::string msg){};
    virtual void on_notice(std::string from, std::string to, std::string msg){};
    virtual void on_nick(std::string from, std::string new_nick){};
    virtual void on_join(std::string from, std::string chan){};
    virtual void on_part(std::string from, std::string chan){};
    virtual void on_msg(std::string from, std::string cmd, std::string msg){};

};

#endif // IRCCLIENTLISTENER_H
