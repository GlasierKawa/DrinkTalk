#ifndef SOCKET_H
#define SOCKET_H

//QT使用winsocket
//https://blog.csdn.net/chenhaifeng2016/article/details/54645670
#include <WinSock2.h>
#include <QDebug>


class Socket
{

public:
    Socket();
    ~Socket();

    void Connect();
    void Disconnect();
    void Send();


private:
    WSADATA wsaData;
    SOCKET cSocket;
    SOCKADDR_IN addr;

};


#endif // SOCKET_H
