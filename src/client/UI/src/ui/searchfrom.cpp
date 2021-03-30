#include "searchfrom.h"
#include "ui_searchfrom.h"



SearchFrom::SearchFrom(int keyId, QWidget *parent) : MoveableFramelessWindow(parent), ui(new Ui::SearchFrom)
{
    ui->setupUi(this);
    //设置阴影
    setShadowEffect(ui->widget_bg);
    //图片适应大小
    ui->label_icon->setScaledContents(true);

    //connect(this,click(),parent,parent->se);

    connect(user.socket, &QTcpSocket::readyRead, this, &SearchFrom::server_reply);

    //查找用户请求
    QJsonObject obj;
    obj.insert("cmd", "searchUserById");
    obj.insert("userId", keyId);
    obj.insert("sendId", user.userId);
    user.send(obj);

    //关闭时执行析构函数
    setAttribute(Qt::WA_DeleteOnClose);
}

SearchFrom::~SearchFrom()
{
    delete ui;
    this->deleteLater();
}

QWidget *SearchFrom::getDragnWidget()
{
    return ui->widget_h;
}


void SearchFrom::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
    qDebug()<<"SearchFrom recive: "<<cmd;
    if (cmd == "searchUserById-reply")
    {
        if(user.obj.value("exist").toBool() == true)
        {
            QJsonObject u = user.obj.value("user").toObject();
            QPixmap p(u.value("iconStr").toString());
            ui->label_icon->setPixmap(p);
            ui->label_name->setText(u.value("userName").toString());
            ui->label_desc->setText(u.value("desc").toString());
            ui->label_id->setNum(u.value("userId").toInt());
            id = u.value("userId").toInt();

            if(user.obj.value("already").toBool() == true)
            {
                ui->pushButton_sendadd->setText("已为好友");
                ui->pushButton_sendadd->setEnabled(false);
                ui->pushButton_sendadd->setStyleSheet("backgroud-color:rgb(150,150,150);");
            }
        }
        else
        {
            ui->pushButton_sendadd->setVisible(false);
        }

        user.clearobj();
    }
    else if(cmd == "addFriend-reply")
    {
        if(user.obj.value("success").toInt() == 1)
        {
//            QMessageBox::information(this, "提示", "请求发送成功");
        }
        else if(user.obj.value("success").toInt() == 0)
        {
//            QMessageBox::information(this, "提示", "用户不存在");
        }
        else
        {
//            QMessageBox::warning(this, "登录提示", "服务器错误");
        }
        user.clearobj();
    }
}


void SearchFrom::on_pushButton_sendadd_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "addFriend");
    obj.insert("sendId", user.userId);
    obj.insert("receiveId", id);
    user.send(obj);

    ui->pushButton_sendadd->setText("请求已发送");
    ui->pushButton_sendadd->setEnabled(false);
    ui->pushButton_sendadd->setStyleSheet("backgroud-color:rgb(150,150,150);");
}







