#pragma once
#include <string>
#include <arpa/inet.h>

using std::string;
class InetAddress
{
public:
    InetAddress(const string &ip,unsigned short port);
    InetAddress(const struct sockaddr_in &addr);
    ~InetAddress();
    const sockaddr_in *getNetsockAddr() const;
    string ip() const;
    unsigned short port() const;

private:
    struct sockaddr_in _addr;
};

