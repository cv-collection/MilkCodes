#include "EventLoop.h"
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
EventLoop::EventLoop(Acceptor &acceptor)
:_acceptor(acceptor) 
,_isLooping(false)
{
    int ret = epoll_create1(0);
    if(ret == -1){
        perror("epoll_create error");
        return ;
    }
    else{
        _epfd = ret;
    }
    _events.resize(1024);

   //监听到说明有新连接
   addEpollNetRead(_acceptor.fd());
}
EventLoop::~EventLoop(){
    if(_epfd){
        close(_epfd);
    }
}
void EventLoop::loop(){
    _isLooping = true;
    while(_isLooping){
        waitforReady();
    }
}
void EventLoop::unloop(){
    _isLooping = false;
}
void EventLoop::setCb_Conn(const TcpCallback &connFunc){
    _onConnection = std::move(connFunc);
}
void EventLoop::setCb_Message(const TcpCallback &messFunc){
    _onMessage= std::move(messFunc);
}
void EventLoop::setCb_CloseConn(const TcpCallback &closedFunc){
    _onClosed= std::move(closedFunc);//传入通知连接关闭的函数
}

void EventLoop::addEpollNetRead(int fd){
    struct epoll_event ev;
    ev.events = EPOLLIN;//监听读事件
    ev.data.fd = fd;
    ::epoll_ctl(_epfd,EPOLL_CTL_ADD,fd,&ev);    
    std::cout<<">>add monitor fd = "<<fd<<std::endl;
}

void EventLoop::delEpollNetRead(int fd){
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    ::epoll_ctl(_epfd,EPOLL_CTL_DEL,fd,&ev);    
}

//处理新连接和旧连接
//1.新连接到了  readyFd = acceptor.fd()
//---新建Tcp连接
//---注册回调函数
//---把Tcp对应的netfd加入监听
//---把当前的Tcp连接加入到map容器中
//---执行连接的回调函数
void EventLoop::handleCb_Conn(){
    int netfd = _acceptor.accept(); 
    TcpConnPtr pconn(new TcpConnection(netfd));
    pconn->setCb_Conn(_onConnection);
    pconn->setCb_Message(_onMessage);
    pconn->setCb_CloseConn(_onClosed);
    addEpollNetRead(netfd);
    _conns.insert(std::make_pair(netfd,pconn));
    //处理回调函数，交给TcpConnection指针(存放在map容器中的)完成
    pconn->handleCb_Conn();
}

//2.旧连接
//---若连接已关闭->删除监视并删除Tcp记录  在已连接队列中找(map容器的find)
//---连接未关闭->执行连接的回调函数
//处理消息---1.删除监视(重点) 2.回显消息 3.删除Tcp连接记录
void EventLoop::handleCb_Message(int netfd){
    auto ret = _conns.find(netfd);
    if(ret == _conns.end()){
        std::cout<<"no connection"<<std::endl;
    }
    else{
        //ret是在map中找到的迭代器，value值(second)就是TcpConnection的连接指针
        auto pconn = ret->second;
        if(pconn->isClosed()){
            pconn->handleCb_CloseConn();
            delEpollNetRead(netfd);//取消监视
            /* _conns.erase(ret);//从已连接的Tcp记录中删除 */
        }
        else{
            ret->second->handleCb_Message();
        }
    }
}

//epollwait--遍历就绪事件队列
void EventLoop::waitforReady(){
   int readyNum = -1;
   do{
       readyNum = ::epoll_wait(_epfd,&(*_events.begin()),_events.size(),5000);
   }while(readyNum == -1 && errno == EINTR);
    
   if(readyNum == -1){
       perror("epoll_wait error");
       return ;
   }
   else if(readyNum == 0){
       std::cout<<">>wait timeout"<<std::endl;
       return ;
   }
   for(int idx =0;idx<readyNum;idx++){
       if(_events[idx].data.fd == _acceptor.fd()){
           //新连接到来
           if(_events[idx].events & EPOLLIN){
                handleCb_Conn();
           }
       }
       else{
            //处理旧连接
           if(_events[idx].events & EPOLLIN){
               handleCb_Message(_events[idx].data.fd);//就绪的事件的文件描述符就是旧连接的netfd
           }
       }
   }
}
