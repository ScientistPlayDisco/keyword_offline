#ifndef __SOCKET_H__
#define __SOCKET_H__

class Socket
{
public:
    Socket();
    Socket(int fd);
    ~Socket();

    int fd() const;
    void shutdownWrite();

private:
    int _fd;
};

#endif
