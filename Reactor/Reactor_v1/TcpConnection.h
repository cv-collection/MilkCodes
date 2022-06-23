#pragma once
#include "InetAddress.h"
#include "Socket.h"
#include "SocketIO.h"
class TcpConnection
{
public:
    TcpConnection(int);
    ~TcpConnection();
    InetAddress getlocalAddr();//本地网络地址
    InetAddress getconnAddr();//连接上服务器的客户端网络地址
    string showconnection() const;//展示连接信息

    string recieve();
    int send(const char *);

private:
    InetAddress _localAddr;
    InetAddress _connAddr;
    Socket _netsock;
    SocketIO srData;//负责数据交换
};

