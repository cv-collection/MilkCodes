#include "TcpConnection.h"
#include "sstream"
#include "string.h"

TcpConnection::TcpConnection(int fd)
:_netsock(fd)
,_localAddr(getlocalAddr())
,_connAddr(getconnAddr())
,srData(fd)
{}
TcpConnection::~TcpConnection(){

}
InetAddress TcpConnection::getlocalAddr(){
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int ret = getsockname(_netsock.fd(),(struct sockaddr *)&addr,
               &len);
    if(ret == -1){
        perror("getsockname error");
    }
    return InetAddress(addr);
}
InetAddress TcpConnection::getconnAddr(){
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int ret = getpeername(_netsock.fd(),(struct sockaddr *)&addr,
               &len);
    if(ret == -1){
        perror("getpeername error");
    }
    return InetAddress(addr);

}
string TcpConnection::showconnection() const{
    std::ostringstream oss;
    oss<<_localAddr.ip()<<":"<<_localAddr.port()
        <<"--accept-->"<<_connAddr.ip()<<":"<<_connAddr.port();
    return oss.str();
}


string TcpConnection::recieve(){
    char buf[1000]={};
    srData.recvn(buf,sizeof(buf));    
    return buf;
}

int TcpConnection::send(const char *buf){
    return srData.sendn(buf,strlen(buf));
}

void TcpConnection::setCb_Conn(const TcpCallback &connFunc){
    _onConnection = std::move(connFunc);
}
void TcpConnection::setCb_Message(const TcpCallback &messFunc){
    _onMessage = std::move(messFunc);
}
void TcpConnection::setCb_CloseConn(const TcpCallback &closedFunc){
    _onClosed = std::move(closedFunc);
}


void TcpConnection::handleCb_Conn(){
    _onConnection(shared_from_this());
}
void TcpConnection::handleCb_Message(){
    _onMessage(shared_from_this());
}
void TcpConnection::handleCb_CloseConn(){
    _onClosed(shared_from_this());
}

bool TcpConnection::isClosed() const{
    char buf[10] ={};
    //通过测试是否还能从netfd中读取数据
    int ret = recv(_netsock.fd(),buf,sizeof(buf),MSG_PEEK);
    if(ret == 0){
        return true;
    }
    else {
        return false;
    }
}

