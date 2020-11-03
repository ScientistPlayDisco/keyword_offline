#include "Thread.h"

using namespace std;

namespace current_thread {
__thread const char* name = "wd thread";
}

using ThreadCallback = Thread::ThreadCallback;

struct ThreadData {
    string _name;
    ThreadCallback _cb;

    ThreadData(const std::string& name, const ThreadCallback& cb)
    : _name(name)
    , _cb(cb)
    {}

    void runInThread() {
        current_thread::name = (_name == std::string()) ? "wd thread" : _name.c_str();
        if (_cb)
            _cb();
    }
};

Thread::Thread(ThreadCallback&& cb, const string& name) 
    : _pthid(0), _isRunning(false), _cb(std::move(cb)), _name(name) {}

Thread::~Thread() {
    if (!_isRunning) pthread_detach(_pthid);
}

void Thread::start() {
    ThreadData* pdata = new ThreadData(_name, _cb);
    pthread_create(&_pthid, nullptr, threadFunc, pdata);
    _isRunning = true;
}

void Thread::join() {
    if (_isRunning) {
        pthread_join(_pthid, nullptr);
        _isRunning = false;
    }
}

void* Thread::threadFunc(void* arg) {
    ThreadData* p = static_cast<ThreadData*>(arg);
    if (p) {
        p->runInThread();
    }
    delete p;
    return nullptr;
}
