#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "Socket.h"
#include "InetAddress.h"
#include "SocketIO.h"
#include "Noncopyable.h"

#include <string>
#include <memory>
#include <functional>
 
using std::string;

class EventLoop;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using TcpConnectionCallback = std::function<void(const TcpConnectionPtr&)>;

class TcpConnection : Noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(int fd, EventLoop* loop);
    ~TcpConnection();

    string receive();
    void send(const string& msg);
    void sendInLoop(const string& msg);

    string toString() const;
    void shutdown();

    //注册回调函数
    void setConnectionCallback(const TcpConnectionCallback& cb) { _onConnection = cb; }
    void setMessageCallback(const TcpConnectionCallback& cb) { _onMessage = cb; }
    void setCloseCallback(const TcpConnectionCallback& cb) { _onClose = cb; }

    //处理回调函数
    void handleConnectionCallback();
    void handleMessageCallback();
    void handleCloseCallback();
    bool isConnectionClose();

private:
    InetAddress getLocalAddr(int fd);
    InetAddress getPeerAddr(int fd);

private:
    EventLoop* _loop;
    Socket _sock;
    SocketIO _socketIo;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    bool _isShutdownWrite;

    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

#endif
