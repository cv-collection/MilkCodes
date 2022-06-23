# 对外接口

## 1.Socket

```c++
int fd() const;//获取当前socket对应的文件描述符
void WriteShutdown();//关闭写端
```



## 2.InetAddr

```c++
const sockaddr_in *getNetsockAddr() const;//获取网络套接字的地址

struct sockaddr_in {//一般是使用port还有ip
    uint8_t sin_len; /* length of structure (16) */
    sa_family_t sin_family; /* AF_INET */
    in_port_t sin_port; /* 16-bit TCP or UDP port number */
    /* network byte ordered */
    struct in_addr sin_addr; /* 32-bit IPv4 address */
    /* network byte ordered */
    char sin_zero[8]; /* unused */
};
```



## 3.Acceptor

```c++
void ready();//accept之前的所有准备工作
void setReuseAddr();//设置socket套接字的属性--忽略TIME_WAIT
void setReusePort();
int accept();//accept会从全连接队列中取出一个连接，构成新的netfd-->Tcp连接OK
int fd() const;//获取当前监听的fd

private://可以不对外开放
    void bind();
    void listen();
```



## 4.SocketIO

```c++
int sendn(const char *buf,int len);
int recvn(char *buf,int len);
```



## 5.TcpConnection

```c++
char *recieve();//从客户端接收数据
int send(const char *);//发送数据给客户端
InetAddress getConnAddr();//获取对端地址
InetAddress getLocalAddr();//获取本地地址
void showConnInfo();//展示连接信息
```

