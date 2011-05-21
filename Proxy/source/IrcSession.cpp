#include "Utils.h"
#include "IrcSession.h"
#include "IrcSessionManager.h"
#include "libjson/libjson.h"

#include <iostream>
#include <sstream>
#include <ctime>

IrcSession::IrcSession(const std::string& id)
    : m_id(id), m_seq(211111)
{
    std::cout << "IrcSession Created, id: " << id << std::endl;
    m_client = new IrcClient(Utils::get_irc_host() , Utils::get_irc_port());
    m_client->add_listener(this);
    stamp();
}

IrcSession::~IrcSession()
{
    if(m_client)
        delete m_client;
}


void IrcSession::connect(std::string nick, std::string user, std::string name, std::string host, std::string ip, std::string device)
{
    m_client->connect(nick, user, name, host, ip, device);
    on_connect();
}

std::string IrcSession::get_id()
{
    return(m_id);
}

IrcClient& IrcSession::get_client()
{
    return(*m_client);
}

/** @brief get_message
  *
  */
std::string IrcSession::get_message()
{
    std::string ret = msgs.front();
    msgs.pop_front();
    return(ret);
}

/** @brief has_messages
  *
  */
bool IrcSession::has_messages()
{
    return(msgs.size() > 0);
}


/** @brief on_msg
  *
  */
void IrcSession::on_msg(std::string from, std::string cmd, std::string msg)
{
    if(cmd == "JOIN")
    {
        if(msg.size() > 1 && msg[0] == ':')
            msg = msg.substr(1);
        on_join(from , msg);
    }
    else if(cmd == "PART")
    {
        if(msg.size() > 1 && msg[0] == ':')
            msg = msg.substr(1);
        on_part(from , msg);
    }
    else if(cmd == "PRIVMSG")
    {
        std::stringstream ss(msg);
        std::string to;
        std::string m;
        ss >> to;
        getline(ss , m);
        unsigned int i = 0;
        while((i < m.size()-1) && (m[i] == ' ' || m[i] == ':'))
        {
            i++;
        }
        std::cout << i << "  " << m.size() << std::endl;
        std::cout << (int)m[i] << "  " << m[i] << std::endl;
        if(i < m.size())
            m = m.substr(i);
        on_privmsg(from , to , m);
    }
    else if(cmd == "NOTICE")
    {
        std::stringstream ss(msg);
        std::string to;
        std::string m;
        ss >> to;
        getline(ss , m);
        unsigned int i = 0;
        while(i < m.size() && (m[i] == ' ' || m[i] == ':'))
            i++;
        if(i && (m.size() > i))
            m = m.substr(i);
        on_notice(from , to , m);
    }
    else if(cmd == "NICK")
    {
        on_nick(from, msg);
    }
    else
    {
        std::string str;
        std::stringstream ss(str);
        ss << "[" << from << "] <" << cmd << "> " << msg;
        JSONNode n(JSON_NODE);
        n.push_back(JSONNode("seq" , m_seq++));
        n.push_back(JSONNode("cmd" , "raw"));
        n.push_back(JSONNode("msg" , ss.str()));
        msgs.push_back(n.write());
    }
}

/** @brief on_part
  *
  */
void IrcSession::on_part(std::string from, std::string chan)
{
    JSONNode n(JSON_NODE);
    n.push_back(JSONNode("seq" , m_seq++));
    n.push_back(JSONNode("cmd" , "part"));
    n.push_back(JSONNode("nick" , from));
    n.push_back(JSONNode("chan" , chan));
    msgs.push_back(n.write());
}

/** @brief on_join
  *
  */
void IrcSession::on_join(std::string from, std::string chan)
{
    JSONNode n(JSON_NODE);
    n.push_back(JSONNode("seq" , m_seq++));
    n.push_back(JSONNode("cmd" , "join"));
    n.push_back(JSONNode("nick" , from));
    n.push_back(JSONNode("chan" , chan));
    msgs.push_back(n.write());
    on_userinfo(from);
}

void IrcSession::on_userinfo(std::string nick)
{
    IrcSession* nick_session = IrcSessionManager::get_instance().get_session_by_nick(nick);
    JSONNode n(JSON_NODE);
    n.push_back(JSONNode("seq" , m_seq++));
    n.push_back(JSONNode("cmd" , "userinfo"));
    if(nick_session)
    {
        UserInfo ui = nick_session->get_client().get_user_info();
        n.push_back(JSONNode("webclient" , "yes"));
        n.push_back(JSONNode("device" , ui.device));
    }
    else
    {
        n.push_back(JSONNode("webclient" , "no"));
    }


    msgs.push_back(n.write());
}

/** @brief on_nick
  *
  */
void IrcSession::on_nick(std::string from, std::string new_nick)
{
    JSONNode n(JSON_NODE);
    n.push_back(JSONNode("seq" , m_seq++));
    n.push_back(JSONNode("cmd" , "nickchange"));
    n.push_back(JSONNode("new" , new_nick));
    n.push_back(JSONNode("old" , from));
    msgs.push_back(n.write());
}

/** @brief on_notice
  *
  */
void IrcSession::on_notice(std::string from, std::string to, std::string msg)
{
    JSONNode n(JSON_NODE);
    n.push_back(JSONNode("seq" , m_seq++));
    n.push_back(JSONNode("cmd" , "notice"));
    n.push_back(JSONNode("nick" , from));
    n.push_back(JSONNode("chan" , to));
    n.push_back(JSONNode("msg" , msg));
    msgs.push_back(n.write());
}

/** @brief on_privmsg
  *
  */
void IrcSession::on_privmsg(std::string from, std::string to, std::string msg)
{
    JSONNode n(JSON_NODE);
    if(msg[0] == 1)     // emote
    {
        n.push_back(JSONNode("cmd" , "emote"));
        unsigned int j = msg.find_last_of(1);
        int i = msg.find(' ')+1;
        msg = msg.substr(i , j-i);
    }
    else
    {
        n.push_back(JSONNode("cmd" , "message"));
    }
    n.push_back(JSONNode("seq" , m_seq++));
    n.push_back(JSONNode("nick" , from));
    n.push_back(JSONNode("chan" , to));
    n.push_back(JSONNode("msg" , msg));
    msgs.push_back(n.write());
}

/** @brief on_connect
  *
  */
void IrcSession::on_connect()
{
    JSONNode n(JSON_NODE);
    n.push_back(JSONNode("seq" , m_seq++));
    n.push_back(JSONNode("cmd" , "connect"));
    n.push_back(JSONNode("id" , m_id));
    msgs.push_back(n.write());
}


void IrcSession::stamp()
{
    m_stamp = std::time(NULL);
}

time_t IrcSession::get_stamp()
{
    return(m_stamp);
}
