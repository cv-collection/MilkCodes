#include "SocketIO.h"
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

SocketIO::SocketIO(int fd)
:_sockfd(fd)
{}
SocketIO::~SocketIO() {}

int SocketIO::sendn(const char *buf,int len){
    int nleft = len;
    int nwrite = 0;
    const char *ptr = buf;
    while(nleft > 0){
        nwrite = write(_sockfd,ptr,nleft);
        if(nwrite == -1 && errno == EINTR){
            continue;
        }
        else if(nwrite != -1){
            break;
        }
        nleft -= nwrite;//控制退出循环的条件
        ptr += nwrite;
    }
    return nleft - nwrite;
}
int SocketIO::recvn(char *buf,int len){
    int nleft = len;//还要读多少
    int nread = 0;//已经读了多少
    char *ptr = buf;//读到哪里了
    while(nleft>0){
        nread = read(_sockfd,ptr,nleft);
        if(nread == -1 && errno == EINTR){
            continue;
        }
        else if(nread != -1){
            break;
        }
        nleft -= nread;//控制退出循环的条件
        ptr += nread;
    }
    return len - nread;

}
int SocketIO::readline(char *buf,int len){
    int left = len - 1;
    char *ptr = buf;
    int ret = 0,total = 0;
    while(left > 0){
        //MSG_PEEK作为属性的recv是为了测试缓冲区是否有数据
        //并不会将缓冲区中的数据进行清空
        ret = recv(_sockfd,ptr,left,MSG_PEEK);
        if(ret == -1 && errno == EINTR){
            continue;
        }
        else if(ret == -1){
            perror("readline error");
            return len - ret;
        } 
        else if(ret == 0){
            break;
        }
        else{
            //真正地读取缓冲区数据
            for(int idx = 0 ;idx < ret ; ++idx){
                if(ptr[idx] == '\n'){//遇到换行符
                    int size = idx + 1;
                    recvn(ptr,size);
                    ptr += size;
                    *ptr = '\0';
                    return total + size;
                }
            }
            recvn(ptr,ret);
            total += ret;
            ptr += ret;
            left -= ret;
        }
    }
    *ptr = '\0';
    return len - ret;
}
