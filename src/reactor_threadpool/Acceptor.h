#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "Socket.h"
#include "InetAddress.h"

class Acceptor
{
public:
    Acceptor(unsigned short port);
    Acceptor(const string& ip, unsigned short port);
    void ready();
    int accept();
    int fd() const { return _listensock.fd(); }
    
private:
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void bind();
    void listen();

private:
    InetAddress _addr;
    Socket _listensock;
};

#endif
