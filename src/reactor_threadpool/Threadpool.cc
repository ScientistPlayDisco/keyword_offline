#include "Threadpool.h"
#include <iostream>

using namespace std;

Threadpool::Threadpool(size_t threadNum, size_t queSize)
: _threadNum(threadNum)
, _queSize(queSize)
, _taskQueue(_queSize)
, _isExit(false)
{
    _threads.reserve(_threadNum);
}

Threadpool::~Threadpool() {
    if (!_isExit) 
        stop();
}

void Threadpool::start() {
    for (size_t idx = 0; idx != _threadNum; ++idx) {
        unique_ptr<Thread> up(new Thread(std::bind(&Threadpool::threadFunc, this)));
        _threads.push_back(move(up));
    }
    for (auto& pthread : _threads) {
        pthread->start();
    }
}

void Threadpool::stop() {
    if (!_isExit) {
        while (!_taskQueue.empty()) {
            sleep(1);
        }
        _isExit = true;
        _taskQueue.wakeup();
        for (auto& pthread : _threads) {
            pthread->join();
        }
    }
}

void Threadpool::addTask(Task&& task) {
    _taskQueue.push(std::move(task));
}

Task Threadpool::getTask() {
    return _taskQueue.pop();
}

void Threadpool::threadFunc() {
    while (!_isExit) {
        Task task = getTask();
        if (task)
            task();
    }
}
