#pragma once
#include "InetAddress.h"
#include "Socket.h"
#include "SocketIO.h"
#include <functional>
#include <memory>

class TcpConnection
:public std::enable_shared_from_this<TcpConnection>
{
public:
    using TcpConnPtr = std::shared_ptr<TcpConnection>;
    using TcpCallback = std::function<void(const TcpConnPtr &)>;
public:
    TcpConnection(int);
    ~TcpConnection();
    string showconnection() const;//展示连接信息
    string recieve();//接收数据
    int send(const char *);//发送数据

    bool isClosed() const;//连接是否已关闭
    //注册回调函数
    void setCb_Conn(const TcpCallback &);
    void setCb_Message(const TcpCallback &);
    void setCb_CloseConn(const TcpCallback &);
    //处理回调函数
    void handleCb_Conn();//通知连接成功
    void handleCb_Message();//信息回显
    void handleCb_CloseConn();//通知连接关闭

private://只需要对showconnection成员函数--类内开放
    InetAddress getlocalAddr();//本地网络地址
    InetAddress getconnAddr();//连接上服务器的客户端网络地址

private:
    Socket _netsock;
    InetAddress _localAddr;
    InetAddress _connAddr;
    SocketIO srData;//负责数据交换
    TcpCallback _onConnection;
    TcpCallback _onClosed;
    TcpCallback _onMessage;

};

