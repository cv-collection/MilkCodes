#include "Acceptor.h"
#include "EventLoop.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

void onConnection(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has connected!" << endl;
}

void onMessage(const TcpConnectionPtr &con)
{
    //回显
    string msg = con->receive();
    cout << "recv msg from client : " << msg << endl;
    con->send(msg);
}

void onClose(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has colsed!" << endl;
}

void test()
{
    Acceptor acceptor("127.0.0.1", 8888);
    acceptor.ready();

    EventLoop loop(acceptor);

    loop.setConnectionCallback(std::move(onConnection));
    loop.setMessageCallback(std::move(onMessage));
    loop.setCloseCallback(std::move(onClose));
    loop.loop();
}

int main(int argc, char **argv)
{
    test();
    return 0;
}

