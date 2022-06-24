#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

void showConn(const TcpConnPtr &conn){
    cout<<conn->showconnection()<<" has connected"<<endl;
}
void showMessage(const TcpConnPtr &conn){
    string msg = conn->recieve();
    cout<<"recieve from client :"<<msg<<endl;
    conn->send("send ok\n\n");
}
void notifyClose(const TcpConnPtr conn){
    cout<<conn->showconnection()<<" has closed"<<endl;
}

void test()
{
    Acceptor acceptor("127.0.0.1", 1234);
    acceptor.ready();//此时处于监听状态

    //三次握手就已经建立，可以创建一条TCP连接
    EventLoop elp(acceptor);
    elp.setCb_Conn(showConn);
    elp.setCb_Message(showMessage);
    elp.setCb_CloseConn(notifyClose);

    //测试EventLoop
    elp.loop();
}
int main()
{
    test();
    return 0;
}

