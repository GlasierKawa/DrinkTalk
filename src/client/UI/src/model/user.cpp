#include "user.h"



User::User()
{
    //新建socket
    socket = new QTcpSocket;
}


User::~User()
{
    socket->deleteLater();
}


void User::connect()
{
    //连接socket
    socket->connectToHost(QHostAddress("IP��ַ"),2222 �˿ں�);
}


void User::receive()
{
    if(obj.empty())
    {
        //ba 为所有返回信息
        QByteArray ba = socket->readAll();
        //qDebug()<<"QByteArray:"<<ba;
        // QString >> QJson
         obj = getJsonObjectFromString(ba);
        //转为json
        //QJsonObject obj = QJsonDocument::fromJson(ba).object();
        qDebug()<<"Receive===============<"<<obj;
    }
}


void User::send(QJsonObject obj)
{
    qDebug()<<"Send===============>"<<obj;
    QByteArray ba = QJsonDocument(obj).toJson();
    socket->write(ba);
}


void User::clearobj()
{
    QJsonObject::iterator it = obj.begin();
    while(it != obj.end())
    {
        obj.erase(it);
    }
}

void User::disconnect()
{
    socket->disconnectFromHost();
}


QJsonObject User::getJsonObjectFromString(const QString jsonString)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data());
//    if( jsonDocument.isNull() ){
//        qDebug()<< "===> please check the string "<< jsonString.toLocal8Bit().data();
//    }
    QJsonObject jsonObject = jsonDocument.object();
    return jsonObject;
}



