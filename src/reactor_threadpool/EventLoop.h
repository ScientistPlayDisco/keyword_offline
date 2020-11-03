#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "MutexLock.h"
#include "Acceptor.h"
#include <sys/epoll.h>

#include <vector>
#include <map>
#include <memory>
#include <functional>

using std::vector;
using std::map;

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using TcpConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using Functor = std::function<void()>;

class EventLoop
{
public:
    EventLoop(Acceptor&);

    void loop();
    void runInLoop(Functor&& cb);
    void unloop();

    void setConnectionCallback(TcpConnectionCallback&& cb) { _onConnection = std::move(cb); }
    void setMessageCallback(TcpConnectionCallback&& cb) { _onMessage = std::move(cb); }
    void setCloseCallback(TcpConnectionCallback&& cb) { _onClose = std::move(cb); }

private:
    int createEpollfd();
    void addEpollReadfd(int fd);
    void delEpollReadfd(int fd);
    void waitEpollfd();
    void handleNewConnection();
    void handleMessage(int fd);

    int createEventfd();
    void handleRead();
    void wakeup();
    void doPendingFunctors();

private:
    int _efd;
    Acceptor& _acceptor;
    vector<struct epoll_event> _evtList;
    map<int, TcpConnectionPtr> _conns;
    bool _isLooping;

    int _eventfd;
    vector<Functor> _pendingFunctors;
    MutexLock _mutex;

    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

#endif
