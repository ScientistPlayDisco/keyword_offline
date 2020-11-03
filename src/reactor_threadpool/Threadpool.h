#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include "TaskQueue.h"
#include "Thread.h"
#include "Task.h"

#include <cstring>
#include <unistd.h>

#include <memory>
#include <vector>

using std::vector;
using std::unique_ptr;

class Threadpool
{
    friend class WorkerThread;
public:
    Threadpool(size_t threadNum, size_t queSize);
    ~Threadpool();

    void start();
    void stop();
    void addTask(Task&& task);

private:
    Task getTask();
    void threadFunc();

private:
    size_t _threadNum;
    size_t _queSize;
    TaskQueue _taskQueue;
    vector<unique_ptr<Thread>> _threads;
    bool _isExit;
};

#endif
