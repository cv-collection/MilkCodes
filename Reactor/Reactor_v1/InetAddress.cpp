#include "InetAddress.h"
InetAddress::InetAddress(const string &ip,unsigned short port){
    _addr.sin_family = AF_INET;//ipv4
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());//点分十进制表示的ip地址转换成32位大端整数
    _addr.sin_port = htons(port);//存储方式改变 主机字节序->网络字节序
}
InetAddress::InetAddress(const struct sockaddr_in &addr)
:_addr(addr)
{
}
InetAddress::~InetAddress() {}
const sockaddr_in *InetAddress::getNetsockAddr() const{
    return &_addr;
}
string InetAddress::ip() const{
    return string(inet_ntoa(_addr.sin_addr));
}
unsigned short InetAddress::port() const{
    return ntohs(_addr.sin_port);
}
