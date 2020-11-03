#include "TcpServer.h"
#include "Threadpool.h"

#include <cstring>

#include <iostream>

using namespace std;

Threadpool* gThreadpool = nullptr;

namespace wd{
class Task {
public:
    Task(const string& msg, const TcpConnectionPtr& conn)
    : _msg(msg)
    , _conn(conn)
    {}

    void process() {
        string response;
        cout << "threadpoll's sub thread " << current_thread::name << endl;
        _conn->sendInLoop(_msg);
    }

private:
    string _msg;
    TcpConnectionPtr _conn;
};
}// end of namespace

void onConnection(const TcpConnectionPtr& conn) {
    cout << ">> " << conn->toString() << "has connected!" << endl;
    conn->send("welcome to server!");
}

void onMessage(const TcpConnectionPtr& conn) {
    string msg = conn->receive();
    cout << ">> receive: " << msg << endl;
    wd::Task task(msg, conn);
    gThreadpool->addTask(bind(&wd::Task::process, task));
}

void onClose(const TcpConnectionPtr& conn) {
    cout << ">> " << conn->toString() << "has closed!" << endl;
}

int main() {
    Threadpool threadpool(4, 10);
    threadpool.start();
    gThreadpool = &threadpool;

    TcpServer server("192.168.5.35", 8888);

    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.setCloseCallback(onClose);
    server.start();

    return 0;
}
