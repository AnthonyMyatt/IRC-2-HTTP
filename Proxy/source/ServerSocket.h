#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <TcpSocket.h>
#include "libjson/libjson.h"


class ServerSocket : public TcpSocket
{
public:
    ServerSocket(ISocketHandler& h);
    virtual ~ServerSocket();
    void OnAccept();
    void OnLine(const std::string& line);
private:
    std::string m_request;
    std::string m_id;
    bool is_admin;
    void handle_first(const std::string& line);
    bool handle_json(const std::string& line);
    bool handle_admin(JSONNode& node);
    bool handle_admin_stats();
    bool handle_admin_users();
    bool handle_admin_stop();
    void send_responses();
	void handle_options();
    void error(std::string msg, std::string cmd = "");
    void error_overload();
    void send_ok();

};

#endif // SERVERSOCKET_H
