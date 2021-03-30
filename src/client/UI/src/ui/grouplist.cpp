#include "grouplist.h"
#include "ui_grouplist.h"

GroupList::GroupList(QWidget *parent) : QWidget(parent), ui(new Ui::GroupList)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(user.socket, &QTcpSocket::readyRead, this, &GroupList::server_reply);
}

GroupList::~GroupList()
{
    delete ui;
}

void GroupList::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
    qDebug()<<"GroupList recive: "<<cmd;
    if (cmd == "createGroup-reply")
    {
        ui->widget->layout()->addWidget(new GroupItem(user.obj.value("group").toObject(), this));
        user.clearobj();
    }
    else if (cmd == "addGroupToList")
    {
        ui->widget->layout()->addWidget(new GroupItem(user.obj.value("group").toObject(), this));
        user.clearobj();
    }
    else if(cmd == "getGroupList-reply")
    {
        QJsonArray gros = user.obj.value("group").toArray();
        for(QJsonArray::iterator it = gros.begin(); it != gros.end(); it++)
        {
            ui->widget->layout()->addWidget(new GroupItem((*it).toObject(), this));
        }
        user.clearobj();
    }
}






