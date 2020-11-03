#include "TcpConnection.h"
#include "InetAddress.h"
#include "EventLoop.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>

#include <sstream>

TcpConnection::TcpConnection(int fd, EventLoop* loop)
: _loop(loop)
, _sock(fd)
, _socketIo(fd)
, _localAddr(getLocalAddr(fd))
, _peerAddr(getPeerAddr(fd))
, _isShutdownWrite(false)
{}

TcpConnection::~TcpConnection() {
    if (!_isShutdownWrite) {
        shutdown();
    }
}

string TcpConnection::receive() {
    char buff[65536] = {0};
    _socketIo.readline(buff, sizeof(buff));
    return string(buff);
}

void TcpConnection::send(const string& msg) {
    _socketIo.writen(msg.c_str(), msg.size());
}

void TcpConnection::sendInLoop(const string& msg) {
    if (_loop)
        _loop->runInLoop(std::bind(&TcpConnection::send, this, msg));
}

void TcpConnection::shutdown() {
    if (!_isShutdownWrite) {
        _isShutdownWrite = true;
        _sock.shutdownWrite();
    }
}

string TcpConnection::toString() const {
    std::ostringstream oss;
    oss << _localAddr.ip() << ":" << _localAddr.port() << "-->"
        << _peerAddr.ip() << ":" << _peerAddr.port();
    return oss.str();
}

InetAddress TcpConnection::getLocalAddr(int fd) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    if (-1 == getsockname(_sock.fd(), (struct sockaddr*)&addr, &len)) 
        perror("getsockname");
    return InetAddress(addr);
}

InetAddress TcpConnection::getPeerAddr(int fd) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    if (-1 == getpeername(_sock.fd(), (struct sockaddr*)&addr, &len)) 
        perror("getsockname");
    return InetAddress(addr);
}

void TcpConnection::handleConnectionCallback() {
    if (_onConnection)
        _onConnection(shared_from_this());
}

void TcpConnection::handleMessageCallback() {
    if (_onMessage)
        _onMessage(shared_from_this());
}

void TcpConnection::handleCloseCallback() {
    if (_onClose)
        _onClose(shared_from_this());
}

bool TcpConnection::isConnectionClose() {
    int nready = -1;
    char buff[128] = {0};
    do {
        nready = ::recv(_sock.fd(), buff, sizeof(buff), MSG_PEEK);
    } while (-1 == nready && errno == EINTR);
    return nready == 0;
}
