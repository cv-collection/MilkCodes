#pragma once
#include "NotCopyable.h"
class Socket
:NotCopyable
{
public:
    Socket();
    explicit Socket(int fd);//防止隐式转换
    ~Socket();
    int fd() const;
    void WriteShutdown();//关闭写端
private:
    int _fd;//文件描述符
};

