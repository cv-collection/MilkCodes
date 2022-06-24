#pragma once
class SocketIO
{
public:
    explicit SocketIO(int);
    ~SocketIO();

    int sendn(const char *buf,int len);
    int recvn(char *buf,int len);
    int readline(char *buf,int len);
private:
    int _sockfd;
};

