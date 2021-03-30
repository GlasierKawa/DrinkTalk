#include "chatgroform.h"
#include "ui_chatgroform.h"

ChatGroForm::ChatGroForm(int groupId, QWidget *parent) : MoveableFramelessWindow(parent), ui(new Ui::ChatGroForm)
{
    ui->setupUi(this);

    id = groupId;
    user.groups[id].open = true;

    connect(user.socket, &QTcpSocket::readyRead, this, &ChatGroForm::server_reply);

    //设置阴影
    setShadowEffect(ui->widget_bg);

    ui->label_groupname->setText(user.groups[id].groupName);

    setAttribute(Qt::WA_DeleteOnClose);

    //获取历史记录请求
    QJsonObject obj;
    obj.insert("cmd", "openGroup");
    obj.insert("userId", user.userId);
    obj.insert("groupId", id);
    user.send(obj);
}


ChatGroForm::~ChatGroForm()
{
    user.groups[id].open = false;
    delete ui;
    this->deleteLater();
}


QWidget *ChatGroForm::getDragnWidget()
{
    return ui->widget;
}

void ChatGroForm::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
    qDebug()<<"ChatGroForm receive: "<<cmd;
    if (cmd == "receiveGroupMessage" && user.obj.value("groupId").toInt() == id && user.groups[id].open == true)
    {
        //音效
        QMediaPlayer * player = new QMediaPlayer;
        player->setMedia(QUrl("qrc:/media/radio/ipmsg.mp3"));
        player->setVolume(50);
        player->play();

        ui->widget_msgs->layout()->addWidget(new Msg(member[user.obj.value("sendId").toInt()],
                                                     user.obj.value("content").toString(),
                                                     user.obj.value("time").toString(),
                                                     this));
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
        user.clearobj();
    }
    else if(cmd == "openGroup-reply" && user.obj.value("groupId").toInt() == id)
    {
        fri mem;
        QJsonArray users = user.obj.value("user").toArray();
        for(QJsonArray::iterator item = users.begin(); item != users.end(); item++)
        {
            mem.userId = (*item).toObject().value("userId").toInt();
            mem.userName = (*item).toObject().value("userName").toString();
            mem.iconStr = (*item).toObject().value("iconStr").toString();
            member[mem.userId] = mem;
            ui->widget_mem->layout()->addWidget(new MemberForm(mem, this));
        }

        QJsonArray msgs = user.obj.value("msg").toArray();
        for(QJsonArray::iterator item = msgs.begin(); item != msgs.end(); item++)
        {
            QJsonObject msg = (*item).toObject();
            ui->widget_msgs->layout()->addWidget(new Msg(member[msg.value("sendId").toInt()],
                                                         msg.value("content").toString(),
                                                         msg.value("time").toString(),
                                                         this));
        }
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
        user.clearobj();
    }
}


void ChatGroForm::on_pushButton_send_clicked()
{
    QString msg = ui->textEdit_input->toPlainText();
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("[yyyy/MM/dd hh:mm:ss]");

    QJsonObject obj;
    obj.insert("cmd", "sendGroupMessage");
    obj.insert("sendId", user.userId);
    obj.insert("groupId",  id);
    obj.insert("content", msg);
    obj.insert("time", current_date);
    user.send(obj);

    ui->widget_msgs->layout()->addWidget(new Msg(user.friends[user.userId], msg, current_date,this));
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
    ui->textEdit_input->clear();
}

void ChatGroForm::on_pushButton_clicked()
{
    setWindowState(Qt::WindowMinimized);
}
