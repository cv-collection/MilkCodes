#pragma once
#include "InetAddress.h"
#include "Socket.h"
class Acceptor
{
public:
    Acceptor(const string &,unsigned short);
    ~Acceptor();
    void ready();
    void setReuseAddr();
    void setReusePort();
    int accept();//accept会从全连接队列中取出一个连接，构成新的netfd-->Tcp连接OK
    int fd() const;

private:
    void bind();
    void listen();

private:
    InetAddress _serverAddr;
    Socket _listensock;
};

