#include "EventLoop.h"
#include "TcpConnection.h"

#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/eventfd.h>

#include <iostream>

using namespace std;

EventLoop::EventLoop(Acceptor& acceptor)
: _efd(createEpollfd())
, _acceptor(acceptor)
, _evtList(1024)    //预先开辟1024个值为0的元素，若使用reserve，则只开辟空间，没有初始元素
, _isLooping(false)
, _eventfd(createEventfd())
{
    addEpollReadfd(_acceptor.fd());
    addEpollReadfd(_eventfd);
}

int EventLoop::createEpollfd() {
    int ret = epoll_create1(0);
    if (-1 == ret) {
        perror("epoll_create1");
    }
    return ret;
}

void EventLoop::loop() {
    _isLooping = true;
    while (_isLooping) 
        waitEpollfd();
}

void EventLoop::unloop() {
    if (_isLooping)
        _isLooping = false;
}

void EventLoop::waitEpollfd() {
    int nready = 0;
    nready = epoll_wait(_efd, &*_evtList.begin(), _evtList.size(), 5000);
    if (-1 == nready && errno == EINTR) {
    } else if (-1 == nready) {
        printf("epoll_wait");
        return;
    } else if (0 == nready) {
        cout << ">> epoll_wait timeout!" << endl;
    } else {
        //扩容操作
        if (_evtList.size() == nready) 
            _evtList.resize(2 * nready);

        for (int idx = 0; idx != nready; ++idx) {
            int fd = _evtList[idx].data.fd;
            if (_acceptor.fd() == fd) {
                //处理新连接
                if (_evtList[idx].events & EPOLLIN) 
                    handleNewConnection();
            } else if (_eventfd == fd) {
                if (_evtList[idx].events & EPOLLIN) { 
                    handleRead();
                    cout << "doPendingFunctors()..." << endl;
                    doPendingFunctors();
                } 
            }
            else {
                //针对于已经建立了连接的处理
                if (_evtList[idx].events & EPOLLIN)  
                    handleMessage(fd);
            }
        }
    }
}

void EventLoop::addEpollReadfd(int fd) {
    struct epoll_event value;
    value.data.fd = fd;
    value.events = EPOLLIN;
    int ret = epoll_ctl(_efd, EPOLL_CTL_ADD, fd, &value);
    if (-1 == ret) 
        perror("epoll_ctl");
}

void EventLoop::delEpollReadfd(int fd) {
    struct epoll_event value;
    value.data.fd = fd;
    int ret = epoll_ctl(_efd, EPOLL_CTL_DEL, fd, &value);
    if (-1 == ret) 
        perror("epoll_ctl");
}

void EventLoop::handleNewConnection() {
    int peerfd = _acceptor.accept();
    addEpollReadfd(peerfd);
    //初始化conn
    TcpConnectionPtr conn(new TcpConnection(peerfd, this));
    //建立TcpConnection的对象，所以此时也应该注册回调函数
    //此时调用的是TcpConnection类的注册回调函数
    conn->setConnectionCallback(_onConnection);
    conn->setMessageCallback(_onMessage);
    conn->setCloseCallback(_onClose);

    //将此conn与peerfd绑定，并添加到map中
    _conns.insert(std::make_pair(peerfd, conn));

    //处理连接建立的事务
    conn->handleConnectionCallback();
}

void EventLoop::handleMessage(int fd) {
    auto iter = _conns.find(fd);
    assert(iter != _conns.end());

    //判断连接是否断开
    auto isClosed = iter->second->isConnectionClose();
    if (isClosed) {
        delEpollReadfd(fd);
        //处理连接关闭的事务
        iter->second->handleCloseCallback();
        _conns.erase(iter);
    } else {
        //处理消息到达的事务
        iter->second->handleMessageCallback();
    }
}

void EventLoop::runInLoop(Functor&& cb) {
    {
        MutexLockGuard autolock(_mutex);
        _pendingFunctors.push_back(std::move(cb));
    }
    wakeup();
}

int EventLoop::createEventfd() {
    int ret = ::eventfd(0, 0);
    if (-1 == ret)
        perror("eventfd");
    return ret;
}

void EventLoop::handleRead() {
    uint64_t howmany = 0;
    int ret = ::read(_eventfd, &howmany, sizeof(howmany));
    if (ret != sizeof(howmany))
        perror("read");
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    int ret = ::write(_eventfd, &one, sizeof(one));
    if (ret != sizeof(one)) 
        perror("write");
}

void EventLoop::doPendingFunctors() {
    vector<Functor> tmp;
    {
        MutexLockGuard autoLock(_mutex);
        tmp.swap(_pendingFunctors);
    }
    for (auto& functor : tmp)
        functor();
}
