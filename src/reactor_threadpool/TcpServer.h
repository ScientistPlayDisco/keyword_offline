#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpConnection.h"

class TcpServer
{
public:
    TcpServer(const string& ip, unsigned short port);
    void start();
    void stop();

    void setConnectionCallback(TcpConnectionCallback&& cb);
    void setMessageCallback(TcpConnectionCallback&& cb);
    void setCloseCallback(TcpConnectionCallback&& cb);

private:
    Acceptor _acceptor;
    EventLoop _loop;
};

#endif
