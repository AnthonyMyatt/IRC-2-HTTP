#include "Utils.h"
#include "ServerSocket.h"
#include "IrcSessionManager.h"
#include "libjson/libjson.h"

#include <sstream>
#include <iostream>
using namespace std;

ServerSocket::ServerSocket(ISocketHandler& h)
    : TcpSocket(h), is_admin(false)
{
    SetLineProtocol();
}

ServerSocket::~ServerSocket()
{
    //dtor
}

/** @brief OnLine
  *
  */
void ServerSocket::OnLine(const std::string& line)
{
    TcpSocket::OnLine(line);
    if(line.size() == 0) {
        return;
    }
    else if(line[0] == 'O' && line[1] == 'P')
    {
        handle_options();
    }
    else if(line[0] == 'P' && line[1] == 'O')
    {
        handle_first(line);
        return;
    }
    else if(line[0] == '{')
    {
        bool b = handle_json(line);
        if(b && !is_admin)
            send_responses();
        SetCloseAndDelete();
    }
}

/** @brief OnAccept
  *
  */
void ServerSocket::OnAccept()
{
    TcpSocket::OnAccept();
}


void ServerSocket::handle_first(const std::string& line)
{
    string str = line;
    stringstream ss(str);
    string method;
    string id;
    ss >> method;
    ss >> id;
    if(id.length() > 1)
        m_id = id.substr(1);
}

bool ServerSocket::handle_json(const std::string& line)
{
    try
    {
        JSONNode n = libjson::parse(line);
        string cmd = n.at("cmd").as_string();
        // handle admin messages
        if(cmd.length() > 0 && cmd[0] == '_')
        {
            return(handle_admin(n));
        }
        if(m_id.size() == 0 && cmd != "connect")
        {
            error("Please connect first", line);
            return(false);
        }
        if(m_id.size() == 0 && IrcSessionManager::get_instance().get_user_count() == Utils::get_max_clients())
        {
            error_overload();
            return(false);
        }
        IrcSession* sess = IrcSessionManager::get_instance().get_session(m_id);
        if(sess == NULL)
        {
            error("Couldn't retreive Irc Session", line);
            return(false);
        }
        m_id = sess->get_id();
        if(cmd == "connect")
        {
            std::string host = GetRemoteHostname();
            std::string ip = GetRemoteAddress();
            if (host == "")
               host = ip;
            sess->connect(n.at("nick").as_string() , n.at("user").as_string() , n.at("name").as_string() , host, ip, n.at("device").as_string());
            send_ok();
        }
        else if(cmd == "quit")
        {
            sess->get_client().quit(n.at("msg").as_string());
            IrcSessionManager::get_instance().remove_session(m_id);
            send_ok();
            return(false); // to prevent the server from trying to retrieve responses
                            // from non-existent session.
        }else if(cmd == "raw")
        {
            sess->get_client().raw(n.at("msg").as_string());
            send_ok();
        }else if(cmd == "update")
        {
            send_ok();
        }else if(cmd == "message")
        {
            sess->get_client().message(n.at("chan").as_string() , n.at("msg").as_string());
            send_ok();
        }else if(cmd == "notice")
        {
            sess->get_client().notice(n.at("chan").as_string() , n.at("msg").as_string());
            send_ok();
        }else if(cmd == "nickchange")
        {
            sess->get_client().nick(n.at("new").as_string());
            send_ok();
        }else if(cmd == "emote")
        {
            sess->get_client().emote(n.at("chan").as_string() , n.at("msg").as_string());
            send_ok();
        }else if(cmd == "join")
        {
            sess->get_client().join(n.at("chan").as_string());
            send_ok();
        }else if(cmd == "part")
        {
            sess->get_client().part(n.at("chan").as_string());
            send_ok();
        }
        else if(cmd == "away")
        {
            sess->get_client().away(n.at("msg").as_string());
            send_ok();
        }
        else
        {
            error("Unsupported Command: " + cmd, line);
            return(false);
        }
    }catch(std::exception& ex)
    {
        string err = "Error Parsing JSON: ";
        err += ex.what();
        error(err, line);
        return(false);
    }
    return(true);
}

void ServerSocket::send_responses()
{
    IrcSession* sess = IrcSessionManager::get_instance().get_session(m_id);
    if(sess == NULL)
    {
        error("Cannot retrieve Session");
        return;
    }
    while(sess->has_messages())
        Send(sess->get_message() + "\r\n");
}

void ServerSocket::send_ok()
{
    std::string web_domain = Utils::get_web_domain();
    Send("HTTP/1.1 200 OK\r\n"
		 "Server: IRC2HTTP\r\n"
		 "Access-Control-Allow-Origin: " + web_domain + "\r\n"
		 "Content-Type: text/plain\r\n\r\n");
}

void ServerSocket::handle_options()
{
        std::string web_domain = Utils::get_web_domain();
	Send("HTTP/1.1 200 OK\r\n"
		 "Server: IRC2HTTP\r\n"
		 "Allow: POST,OPTIONS\r\n"
		 "Content-Length: 0\r\n"
		 "Keep-Alive: timeout=1, max=100\r\n"
		 "Connection: Keep-Alive\r\n"
		 "Content-Type: text/plain\r\n"
		 "Access-Control-Allow-Origin: " + web_domain + "\r\n"
		 "Access-Control-Allow-Methods: POST,OPTIONS\r\n"
		 "Access-Control-Allow-Headers: x-requested-with, content-type, accept\r\n"
		 "Access-Control-Max-Age: 1728000\r\n\r\n");
}

void ServerSocket::error(std::string msg, std::string cmd)
{
    string str;
    stringstream ss(str);
    ss << "HTTP/1.1 400 BAD REQUEST" << endl << endl;
    ss << "ID: " << m_id << endl;
    ss << msg << endl;
    if(cmd.size())
    {
        ss << "Recieved Command: " << endl;
        ss << cmd << endl;
    }
    Send(ss.str());
}

void ServerSocket::error_overload()
{
    string str;
    stringstream ss(str);
    ss << "HTTP/1.1 503 Service Unavailable" << endl << endl;
    ss << "Server overloaded! Please try again in a few moments." << endl;
    Send(ss.str());
}

bool ServerSocket::handle_admin(JSONNode& n)
{
    is_admin = true;
    string cmd = n.at("cmd").as_string();
    if(cmd == "_connect")
    {
        return(true);
    }
    else if(cmd == "_users")
    {
        return(handle_admin_users());
    }
    else if(cmd == "_stats")
    {
        return(handle_admin_stats());
    }
    else if(cmd == "_stop")
    {
        return(handle_admin_stop());
    }
    return(true);
}

bool ServerSocket::handle_admin_stats()
{
    JSONNode n(JSON_NODE);
    n.push_back(JSONNode("cmd" , "_stats"));
    n.push_back(JSONNode("count" , IrcSessionManager::get_instance().get_user_count()));
    send_ok();
    Send(n.write() + "\n");
    return(true);
}

bool ServerSocket::handle_admin_users()
{
    std::vector<UserInfo> v = IrcSessionManager::get_instance().get_users();
    JSONNode n(JSON_NODE);
    n.push_back(JSONNode("cmd" , "_users"));
    JSONNode narr(JSON_ARRAY);
    narr.set_name("list");
    for(unsigned int i = 0 ; i < v.size() ; i++)
    {
        JSONNode nn(JSON_NODE);
        nn.push_back(JSONNode("nick" , v[i].nick));
        nn.push_back(JSONNode("name" , v[i].name));
        nn.push_back(JSONNode("user" , v[i].user));
        nn.push_back(JSONNode("host" , v[i].host));
        nn.push_back(JSONNode("device" , v[i].device));
        narr.push_back(nn);
    }
    n.push_back(narr);
    send_ok();
    Send(n.write() + "\n");
    return(true);
}

bool ServerSocket::handle_admin_stop()
{
    Utils::httpd_stop();
    send_ok();
    return(true);
}
