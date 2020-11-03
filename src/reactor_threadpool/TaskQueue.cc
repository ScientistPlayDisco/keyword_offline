#include "TaskQueue.h"
#include <iostream>

using namespace std;

TaskQueue::TaskQueue(size_t queSize) 
: _que(), _queSize(queSize), _mutex(), _notFull(_mutex), _notEmpty(_mutex), _flag(true) {}

bool TaskQueue::full() {
    return _que.size() == _queSize;
}

bool TaskQueue::empty() {
    return _que.size() == 0;
}

void TaskQueue::push(const ElemType& elem) {
    //先加锁
    /* _mutex.lock(); */
    //RAII机制
    MutexLockGuard autolock(_mutex);
    //若满则等待
    //若将while换成if则有可能发生虚假唤醒的情况，换成while后可以进行第二次检测
    //原因是一个notify操作可能唤醒好几个线程（多线程编程中）
    while (full()) {
        _notFull.wait();
    }
    _que.push(elem);
    //通知_notEmpty已经放入elem，可以pop了
    _notEmpty.notify();
    /* _mutex.unlock(); */
}

ElemType TaskQueue::pop() {
    /* _mutex.lock(); */
    //RAII机制
    MutexLockGuard autolock(_mutex);
    while (_flag && empty()) {
        _notEmpty.wait();
    }
    if (_flag) {
        ElemType elem = _que.front();
        _que.pop();
        _notFull.notify();
        return elem;
    } else {
        return nullptr;
    }
    /* _mutex.unlock(); */
}

void TaskQueue::wakeup() {
    _flag = false;
    _notEmpty.notifyAll();
}
