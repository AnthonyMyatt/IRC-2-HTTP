#ifndef HTTPD_H
#define HTTPD_H

#include <ListenSocket.h>
#include "ServerSocket.h"


class Httpd
{
public:
    Httpd();
    virtual ~Httpd();

    void start();
    void run();
    void stop();

private:
    ListenSocket<ServerSocket>* m_lsock;
};

#endif // HTTPD_H
