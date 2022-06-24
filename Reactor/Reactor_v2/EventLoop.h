#pragma once
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "Acceptor.h"
#include "TcpConnection.h"

using std::vector;
using std::map;

using TcpConnPtr = std::shared_ptr<TcpConnection>;
using TcpCallback = std::function<void(const TcpConnPtr &)>;
class EventLoop
{
public:
    EventLoop(Acceptor &);
    ~EventLoop();
    void loop();//开始
    void unloop();//结束
    //注册回调函数
    void setCb_Conn(const TcpCallback &);
    void setCb_Message(const TcpCallback &);
    void setCb_CloseConn(const TcpCallback &);
    //处理回调函数，交给TcpConnection指针(存放在map容器中的)完成
    void handleCb_Conn();
    void handleCb_Message(int);//处理旧连接---1.删除监视(未连接),并释放Socket对象 2.回显消息

    void waitforReady();
    //加入监听
    void addEpollNetRead(int);
    //取消监听
    void delEpollNetRead(int);
private:
    int _epfd;
    Acceptor &_acceptor;
    bool _isLooping;
    vector<struct epoll_event> _events;//用容器记录监听的事件
    map<int,TcpConnPtr> _conns;//记录一个netfd对应一个Tcp连接
    TcpCallback _onConnection;
    TcpCallback _onClosed;
    TcpCallback _onMessage;
};

