#include "InetAddress.h"

#include <cstring>

InetAddress::InetAddress(unsigned short port) {
    memset(&_addr, 0, sizeof(struct sockaddr_in));
    _addr.sin_family = AF_INET;
    //主机字节序转成网络字节序列
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = INADDR_ANY;
}

InetAddress::InetAddress(const string& ip, unsigned short port) {
    memset(&_addr, 0, sizeof(struct sockaddr_in));
    _addr.sin_family = AF_INET;
    //主机字节序转成网络字节序列
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

InetAddress::InetAddress(const struct sockaddr_in& addr) : _addr(addr) {}

string InetAddress::ip() const {
    //inet_ntoa()将网络字节序的32位二进制数值转换为点分十进制的二进制
    return string(inet_ntoa(_addr.sin_addr));
}

unsigned short InetAddress::port() const {
    //主机字节序与厂商有关，可能为大端模式，也可能为小端模式
    //网络字节序一般为大端模式
    //将网络字节序转换为主机字节序
    return ntohs(_addr.sin_port);
}
