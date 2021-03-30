#include "socket.h"
//https://blog.csdn.net/chenhaifeng2016/article/details/54645670
#include <WinSock2.h>
#include <QDebug>


Socket::Socket()
{
    //确定协议版本
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (HIBYTE(wsaData.wVersion) != 2 || LOBYTE(wsaData.wVersion) != 2)
    {
        qDebug() << "Protocol err.";
        return;
    }
    qDebug() << "Protocol success."  ;

    //创建Socket
    cSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (cSocket == SOCKET_ERROR)
    {
        qDebug() << "Socket err."  ;
        WSACleanup();//清理协议版本
        return;
    }
     qDebug() << "Socket success."  ;

    //获取服务器协议簇
    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;//same as the 1st ref of sosket function
    addr.sin_addr.S_un.S_addr = inet_addr("192.168.3.7");//IP
    addr.sin_port = htons(9527);//端口号
}


Socket::~Socket()
{
    //断开连接
    closesocket(cSocket);
    qDebug() << "Creak connect."  ;

    //清理协议版本信息
    WSACleanup();
    qDebug() << "Clear protocol info."  ;
}


void Socket::Connect()
{
    //连接
    int r = connect(cSocket, (sockaddr*)& addr, sizeof addr);
    if (r == -1)
    {
         qDebug() << "Connect err."  ;
        closesocket(cSocket);//断开
        WSACleanup();//清理协议版本
        return;
    }
     qDebug() << "Connect success."  ;
}


void Socket::Disconnect()
{
    closesocket(cSocket);
    qDebug() << "Creak connect."  ;
}


void Socket::Send()
{
    char buff[1024]="Hello QT!";
    qDebug() << "Send massage: ";
    send(cSocket, buff, strlen(buff), NULL);

}
