#include "msg.h"
#include "ui_msg.h"

/*
    "cmd": "reciveMessage", //在线接受
    "sendId": 100,
    "content": "消息内容",
    "time": "2021年3月16日 15:03:40"
*/

Msg::Msg(fri u,
         QString msg,
         QString time,
         QWidget *parent) : QWidget(parent), ui(new Ui::Msg)
{
    ui->setupUi(this);
    //头像
    QPixmap p(u.iconStr);
    ui->label_vat->setPixmap(p);
    ui->label_vat->setScaledContents(true);
    //时间
    ui->label_time->setText(time);
    //昵称
    ui->label_username->setText(u.userName);
    //消息
    ui->label_msg->setText(msg);
    ui->label_msg->setWordWrap(true);
    if(msg == "[戳一戳]")
    {
        QMediaPlayer * player = new QMediaPlayer;
        player->setMedia(QUrl("qrc:/media/radio/hua.mp3"));
        player->setVolume(25);
        player->play();
    }
    setAttribute(Qt::WA_DeleteOnClose);

}

Msg::~Msg()
{
    delete ui;
}
