#include "Httpd.h"
#include "Utils.h"
#include "IrcSessionManager.h"

#include <iostream>



Httpd::Httpd()
    : m_lsock(NULL)
{
    //ctor
}

Httpd::~Httpd()
{
    if(m_lsock != NULL)
        delete m_lsock;
}

/** @brief start
  *
  */
void Httpd::start()
{
    unsigned int port = Utils::get_server_port();
    m_lsock = new ListenSocket<ServerSocket>(Utils::get_socket_handler());
    if(m_lsock->Bind(port))
    {
        std::cerr << "Error Binding ListenSocket" << std::endl;
        delete m_lsock;
        m_lsock = NULL;
        return;
    }
    m_lsock->SetDeleteByHandler();
    Utils::get_socket_handler().Add(m_lsock);
    run();
}

/** @brief run
  *
  */
void Httpd::run()
{
    std::cout << "Httpd Running: " << std::endl;
    while(!Utils::is_httpd_stop())
    {
        Utils::get_socket_handler().Select(1 , 0);
        IrcSessionManager::get_instance().check_timeout();
    }
    m_lsock->SetCloseAndDelete();
    m_lsock = NULL;
}

