#include "litteriem.h"
#include "ui_litteriem.h"



LitterIem::LitterIem(int userId, QWidget *parent) : QWidget(parent),  ui(new Ui::LitterIem)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    id = userId;

    //设置头像
    QPixmap p(user.friends[id].iconStr);
    ui->label_pic->setPixmap(p);
    ui->label_id->setNum(user.friends[id].userId);
    ui->label_name->setText(user.friends[id].userName);
    ui->label_signal->setText(user.friends[id].desc);

    int msgnum = user.friends[id].msgNum;
    ui->label_msgnum->setNum(msgnum);
    if(msgnum)
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
    connect(user.socket, &QTcpSocket::readyRead, this, &LitterIem::server_reply);

//    connect(ui->pushButton_login,SIGNAL(clicked()),this,SLOT(doLoginButClick()));
}


LitterIem::~LitterIem()
{
    delete ui;
}


void LitterIem::mouseReleaseEvent(QMouseEvent *ev)
{
    emit clicked();
}


void LitterIem::doChatClick()
{
    if(!user.friends[id].open)
    {
        user.friends[id].msgNum = 0;
        ui->label_msgnum->setNum(0);
        ui->label_msgnum->setVisible(false);
        ChatForm*c = new ChatForm(id);
        c->show();
    }
}


void LitterIem::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
//    qDebug()<<"Litteriem recive: "<<cmd;
    if (cmd == "receiveMessage" && user.obj.value("sendId").toInt() == id && user.friends[id].open == false)
    {
        //音效
        QMediaPlayer * player = new QMediaPlayer;
        player->setMedia(QUrl("qrc:/media/radio/msg2.mp3"));
        player->setVolume(50);
        player->play();

        user.friends[id].msgNum += 1;
        int msgnum = user.friends[id].msgNum;
        ui->label_msgnum->setNum(msgnum);
        ui->label_msgnum->setVisible(true);

        user.clearobj();
    }
}
