#include "noticelist.h"
#include "ui_noticelist.h"

NoticeList::NoticeList(QWidget *parent) : QWidget(parent), ui(new Ui::NoticeList)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(user.socket, &QTcpSocket::readyRead, this, &NoticeList::server_reply);
}

NoticeList::~NoticeList()
{
    delete ui;
}

void NoticeList::addNoticeList(QJsonArray notices)
{
    for(QJsonArray::iterator item = notices.begin(); item != notices.end(); item++)
    {
        ui->widget->layout()->addWidget(new NoticeItem((*item).toObject(), this));
    }
}

void NoticeList::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
    qDebug()<<"NoticeList recive: "<<cmd;
    if (cmd == "addFriendNotification")
    {
        //音效
        QMediaPlayer * player = new QMediaPlayer;
        player->setMedia(QUrl("qrc:/media/radio/kk.mp3"));
        player->setVolume(50);
        player->play();

        ui->widget->layout()->addWidget(new NoticeItem(user.obj.value("user").toObject(), this));
        user.clearobj();
    }
    else if(cmd == "readAddFriendNotification-reply")
    {
        addNoticeList(user.obj.value("user").toArray());
        user.clearobj();

        //请求获取群聊列表信息
        QJsonObject obj;
        obj.insert("cmd", "getGroupList");
        obj.insert("userId", user.userId);
        user.send(obj);
    }
}
