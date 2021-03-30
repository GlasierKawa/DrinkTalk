#include "noticeitem.h"
#include "ui_noticeitem.h"

NoticeItem::NoticeItem(QJsonObject u, QWidget *parent) : QWidget(parent), ui(new Ui::NoticeItem)
{
    ui->setupUi(this);
    ui->litter_main->setStyleSheet("QWidget:hover{background-color: rgb(178, 220, 255);}");

    id = u.value("userId").toInt();
    QPixmap p(u.value("iconStr").toString());
    ui->label_pic->setPixmap(p);
    ui->label_id->setNum(u.value("userId").toInt());
    ui->label_name->setText(u.value("userName").toString());
    setAttribute(Qt::WA_DeleteOnClose);
}


NoticeItem::~NoticeItem()
{
    delete ui;
}


void NoticeItem::on_pushButto_ig_clicked()
{
    this->deleteLater();
}

void NoticeItem::on_pushButton_ok_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "addFriendConfirm");
    obj.insert("sendId",id);
    obj.insert("receiveId", user.userId);
    user.send(obj);
    this->deleteLater();
}





