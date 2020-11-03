#ifndef __TASKQUEUE_H__
#define __TASKQUEUE_H__

#include "Condition.h"
#include "MutexLock.h"
#include "Task.h"
#include <queue>

using ElemType = Task;

class TaskQueue
{
public:
    TaskQueue(size_t);
    void push(const ElemType&);
    ElemType pop();
    bool full();
    bool empty();
    void wakeup();

private:
    std::queue<ElemType> _que;
    size_t _queSize;
    MutexLock _mutex;
    Condition _notFull;
    Condition _notEmpty;
    bool _flag;
};

#endif
