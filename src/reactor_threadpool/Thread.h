#ifndef __THREAD_H__
#define __THREAD_H__

#include "Noncopyable.h"

#include <pthread.h>

#include <string>
#include <functional>

namespace current_thread {
extern __thread const char* name;
}

class Thread : Noncopyable {
public:
    using ThreadCallback = std::function<void()>;
    Thread(ThreadCallback&& cb, const std::string& name = std::string());
    ~Thread();

    void start();
    void join();

    pthread_t getThreadId() const { return _pthid; }
    std::string getThreadName() const { return _name; }

private:
    static void* threadFunc(void* arg);

private:
    pthread_t _pthid;
    bool _isRunning;
    ThreadCallback _cb;
    std::string _name;
};

#endif
