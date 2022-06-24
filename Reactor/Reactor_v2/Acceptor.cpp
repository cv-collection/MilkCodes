#include "Acceptor.h"
#include <stdio.h>

Acceptor::Acceptor(const string &ip,unsigned short port)
:_serverAddr(ip,port)
{
}
Acceptor::~Acceptor(){}
void Acceptor::ready(){
    setReuseAddr();
    setReusePort();
    bind();
    listen();
}
//设置可重用，忽视TIME_WAIT时间
void Acceptor::setReuseAddr(){
    int on = 1;
    int ret = setsockopt(_listensock.fd(),SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    if(ret == -1){
        perror("setReuseAddr error");
        return ;
    }
}
void Acceptor::setReusePort(){
    int on = 1;
    int ret = setsockopt(_listensock.fd(),SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on));
    if(ret == -1){
        perror("setReusePort error");
        return ;
    }
}
void Acceptor::bind(){
    int ret = ::bind(_listensock.fd(),
         (struct sockaddr *)_serverAddr.getNetsockAddr(),
         sizeof(struct sockaddr));
    if(ret == -1){
        perror("bind error");
        return ;
    }
}
void Acceptor::listen(){
    ::listen(_listensock.fd(),10);
}
int Acceptor::accept(){
    int netfd = ::accept(_listensock.fd(),nullptr,nullptr);
    if(netfd == -1){
        perror("accept error");
    }
    return netfd;
}
int Acceptor::fd() const{
    return _listensock.fd();
}
