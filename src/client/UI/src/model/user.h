#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QWidget>

#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <map>
#include <QDomElement>



typedef struct {
    int userId;
    QString userName;
    QString iconStr;
    QString desc;
    bool online;
    bool open;
    int msgNum;
}fri;


typedef struct {
    int groupId;
    QString groupName;
    int ownerId;
    int msgNum;
    bool open;
}gro;


// 用于全局变量 保存socket username password
class User
{

public:
    User();
    ~User();

    //连接
    void connect();
    //接收
    void receive();
    //发送
    void send(QJsonObject obj);
    //清空obj
    void clearobj();
    //断开
    void disconnect();
    // QString >> QJson
    QJsonObject getJsonObjectFromString(const QString jsonString);

    QJsonObject obj;
    QTcpSocket *socket;

    int userId;
    QString userName;
    QString password;
    QString iconStr;
    QString desc;

    std::map<int, fri>friends;
    std::map<int, gro>groups;
};



#endif // TCPSOCKET_H
