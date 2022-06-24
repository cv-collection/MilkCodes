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
public:
	string showconnection() const;//展示连接信息
	string recieve();//接收数据
	int send(const char *);//发送数据

	bool isClosed() const;//连接是否已关闭
	//注册回调函数
	void setCb_Conn(const TcpCallback &);
	void setCb_Message(const TcpCallback &);
	void setCb_CloseConn(const TcpCallback &);
	//处理回调函数
	void handleCb_Conn();
	void handleCb_Message();
	void handleCb_CloseConn();

private://只需要对showconnection成员函数--类内开放
	InetAddress getlocalAddr();//本地网络地址
	InetAddress getconnAddr();//连接上服务器的客户端网络地址
```



## 6.EventLoop

> 与TcpConnect之间的关联性

1. ​	EventLoop注册回调函数（传入的参数是TcpConnect的指针）

2. ​    EventLoop中处理回调函数的时候，有部分是嵌套调用TcpConnect类中的回调函数

   ​	EventLoop监视到新连接时**调用pconn的回调函数handleCb_Conn**，监视到旧连接时判断是否对端已经断开->断开则调用pconn的handleCb_Closed；否则就是处理消息，调用pconn的handleCb_Message

   > 解释一下注册回调函数和处理回调函数
   >
   > 注册回调函数是把形参（function存储的函数）赋值给类中的某个函数
   >
   > 而处理回调函数就是调用这个注册后的函数

3. ​    两者均可以用Acceptor进行初始化

```c++
//注册回调函数
void setCb_Conn(const TcpCallback &);
void setCb_Message(const TcpCallback &);
void setCb_CloseConn(const TcpCallback &);

//处理新连接---1.建立连接 2.注册回调函数 3.新连接netfd加入监听,加入Tcp连接容器 4.执行回调函数
void handleCb_Conn();
//处理旧连接---1.删除监视，并释放socket对象(未连接) 2.回显消息 
void handleCb_Message(int);

void waitforReady();
//加入/删除监听
void addEpollNetRead(int);
void delEpollNetRead(int);
```

<img align = "left" src = 'https://i.bmp.ovh/imgs/2022/06/24/898ae21e915499a6.png' />

<img align = "left" src = 'https://i.bmp.ovh/imgs/2022/06/24/2f4b0f2a2697d3bc.png' />

