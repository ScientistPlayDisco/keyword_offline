#include "SocketIO.h"

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>

SocketIO::SocketIO(int fd) : _fd(fd) {}

int SocketIO::readn(char* buff, int len) {
    int left = len;
    char* p = buff;
    while (left > 0) {
        int ret = ::read(_fd, p, left);
        if (-1 == ret && errno == EINTR) {
            continue;
        } else if (-1 == ret) {
            perror("read");
            return len - left;
        } else if (0 == ret) {
            return len - left;
        } else {
            left -= ret;
            p += ret;
        }
    }
    return len - left;
}

int SocketIO::recvPeek(char* buff, int len) {
    int ret;
    do {
        ret = ::recv(_fd, buff, len, MSG_PEEK);
    } while (-1 == ret && errno == EINTR);
    return ret;
}

int SocketIO::writen(const char* buff, int len) {
    int left = len;
    const char* p = buff;
    while (left > 0) {
        int ret = ::write(_fd, p, left);
        if (-1 == ret && errno == EINTR) {
            continue;
        } else if (-1 == ret) {
            perror("write");
            return len - left;
        } else {
            left -= ret;
            p += ret;
        }
    }
    return len - left;
}

int SocketIO::readline(char* buff, int maxlen) {
    int left = maxlen - 1;
    char* p = buff;
    int ret;
    int total = 0;
    while (left > 0) {
        ret = recvPeek(p, left);
        for (int idx = 0; idx != ret; ++idx) {
            if ('\n' == p[idx]) {
                int sz = idx + 1;
                readn(p, sz);
                total += sz;
                p += sz;
                *p = '\0';
                return total;
            }
        }
        readn(p, ret);
        left -= ret;
        p += ret;
        total += ret;
    }
    *p = '\0';
    return total;
}
