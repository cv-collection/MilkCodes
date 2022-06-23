#include "TcpConnection.h"
#include "sstream"
#include "string.h"

TcpConnection::TcpConnection(int fd)
:_localAddr(getlocalAddr())
,_connAddr(getconnAddr())
,_netsock(fd)
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







