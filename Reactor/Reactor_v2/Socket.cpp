#include "Socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

Socket::Socket(){
    _fd = socket(AF_INET,SOCK_STREAM,0);    
    if(_fd < 0){
        perror("socker error");
        return ;
    }
}
Socket::Socket(int fd){
    _fd = fd;
}
Socket::~Socket(){
    close(_fd);
}
int Socket::fd() const{//向外提供访问成员的接口--RAII的思想
    return _fd;
}

void Socket::WriteShutdown(){
    int ret = shutdown(_fd,SHUT_WR);
    if(ret){
        perror("shutdon write error");
        return ;
    }
}
