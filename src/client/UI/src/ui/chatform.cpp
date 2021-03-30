#include "chatform.h"
#include "ui_chatform.h"



ChatForm::ChatForm(int userId, QWidget *parent) : MoveableFramelessWindow(parent), ui(new Ui::ChatForm)
{
    ui->setupUi(this);

    id = userId;
    user.friends[id].open = true;

    connect(user.socket, &QTcpSocket::readyRead, this, &ChatForm::server_reply);

    //设置阴影
    setShadowEffect(ui->widget_bg);
    ui->label_username->setText(user.friends[id].userName);

    setAttribute(Qt::WA_DeleteOnClose);

    //获取历史记录请求
    QJsonObject obj;
    obj.insert("cmd", "getMessageRecord");
    obj.insert("userId1", user.userId);
    obj.insert("userId2", user.friends[id].userId);
    user.send(obj);
}


ChatForm::~ChatForm()
{
    user.friends[id].open = false;
    delete ui;
    this->deleteLater();
}


QWidget *ChatForm::getDragnWidget()
{
    return ui->widget;
}


void ChatForm::on_pushButton_send_clicked()
{
    QString msg = ui->textEdit_input->toPlainText();
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("[yyyy/MM/dd hh:mm:ss]");

    QJsonObject obj;
    obj.insert("cmd", "sendMessage");
    obj.insert("sendId", user.userId);
    obj.insert("receiveId",  user.friends[id].userId);
    obj.insert("content", msg);
    obj.insert("time", current_date);
    user.send(obj);

    ui->widget_msgs->layout()->addWidget(new Msg(user.friends[user.userId], msg, current_date,this));
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
    ui->textEdit_input->clear();
}



void ChatForm::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
    qDebug()<<"Chatform receive: ";
    if (cmd == "receiveMessage" && user.obj.value("sendId").toInt() == id && user.friends[id].open == true)
    {
        //音效
        QMediaPlayer * player = new QMediaPlayer;
        player->setMedia(QUrl("qrc:/media/radio/msg2.mp3"));
        player->setVolume(50);
        player->play();

        ui->widget_msgs->layout()->addWidget(new Msg(user.friends[user.obj.value("sendId").toInt()],
                                                     user.obj.value("content").toString(),
                                                     user.obj.value("time").toString(),
                                                     this));
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
        user.clearobj();
    }
    else if(cmd == "getMessageRecord-reply" && id == user.obj.value("sendId").toInt())
    {
        QJsonArray msgs = user.obj.value("message").toArray();

            for(QJsonArray::iterator item = msgs.begin(); item != msgs.end(); item++)
            {
                QJsonObject msg = (*item).toObject();
                ui->widget_msgs->layout()->addWidget(new Msg(user.friends[msg.value("sendId").toInt()],
                                                             msg.value("content").toString(),
                                                             msg.value("time").toString(),
                                                             this));
            }
            ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());

        user.clearobj();
    }
}

void ChatForm::on_pushButton_min_clicked()
{
    setWindowState(Qt::WindowMinimized);
}
