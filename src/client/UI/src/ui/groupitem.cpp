#include "groupitem.h"
#include "ui_groupitem.h"

GroupItem::GroupItem(QJsonObject g, QWidget *parent) : QWidget(parent), ui(new Ui::GroupItem)
{
    ui->setupUi(this);
    ui->label_pic->setScaledContents(true);
    ui->litter_main->setStyleSheet("QWidget:hover{background-color: rgb(178, 220, 255);}");

    gro group;
    group.groupId = g.value("groupId").toInt();
    group.ownerId = g.value("ownerId").toInt();
    group.groupName = g.value("groupName").toString();
    group.open = false;
    group.msgNum = g.value("msgNum")==NULL ? 0 : g.value("msgNum").toInt();

    groupId = group.groupId;
    ownerId = group.ownerId;
    user.groups[groupId] = group;

    ui->label_name->setText(group.groupName);
    ui->label_signal->setNum(group.groupId);
    ui->label_msgnum->setNum(group.msgNum);
    if(group.msgNum)
    {
        ui->label_msgnum->setVisible(true);
    }
    else
    {
        ui->label_msgnum->setVisible(false);
    }

    //打开聊天窗口槽函数
    connect(this,SIGNAL(clicked()),this,SLOT(doChatClick()));
    //
    connect(user.socket, &QTcpSocket::readyRead, this, &GroupItem::server_reply);

    setAttribute(Qt::WA_DeleteOnClose);
}

GroupItem::~GroupItem()
{
    delete ui;
}

void GroupItem::mouseReleaseEvent(QMouseEvent *ev)
{
    emit clicked();
}

void GroupItem::doChatClick()
{
    if(!user.groups[groupId].open)
    {
        user.groups[groupId].msgNum = 0;
        ui->label_msgnum->setNum(0);
        ui->label_msgnum->setVisible(false);
        ChatGroForm*c = new ChatGroForm(groupId);
        c->show();
    }
}

void GroupItem::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
//    qDebug()<<"GroupItem recive: "<<cmd;
    if (cmd == "receiveGroupMessage" && user.obj.value("groupId").toInt() == groupId && user.groups[groupId].open == false)
    {
        //音效
        QMediaPlayer * player = new QMediaPlayer;
        player->setMedia(QUrl("qrc:/media/radio/ipmsg.mp3"));
        player->setVolume(50);
        player->play();

        user.groups[groupId].msgNum += 1;
        int msgnum = user.groups[groupId].msgNum;
        ui->label_msgnum->setNum(msgnum);
        ui->label_msgnum->setVisible(true);

        user.clearobj();
    }
}


