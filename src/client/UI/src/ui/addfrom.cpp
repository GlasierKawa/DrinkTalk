#include "addfrom.h"
#include "ui_addfrom.h"

AddFrom::AddFrom(QWidget *parent) : MoveableFramelessWindow(parent), ui(new Ui::AddFrom)
{
    ui->setupUi(this);
    setShadowEffect(ui->widget_bg);
    ui->label_s->setVisible(false);
    connect(user.socket, &QTcpSocket::readyRead, this, &AddFrom::server_reply);
    setAttribute(Qt::WA_DeleteOnClose);
}


AddFrom::~AddFrom()
{
    delete ui;
    this->deleteLater();
}


QWidget *AddFrom::getDragnWidget()
{
    return ui->widget;
}


void AddFrom::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
    qDebug()<<"AddFrom recive: "<<cmd;
    if (cmd == "register-reply")
    {
        ui->label_s->setVisible(true);
        ui->label_s->setText(tr("注册成功, 账号id为:%1").arg(user.obj.value("userId").toInt()));
        ui->label_no->setText("");
        user.clearobj();
    }
}


void AddFrom::on_pushButton_reg_clicked()
{
    //user类私有成员变量 获取名称密码 发送登录
    user.userName = ui->lineEdit_name->text();
    user.password = ui->lineEdit_key->text();

    if(user.userName == "")
    {
        ui->label_no->setText("*请输入用户名");
    }
    else if(user.password == "")
    {
        ui->label_no->setText("*请输入密码");
    }
    else
    {
        QJsonObject obj;
        obj.insert("cmd", "register");
        obj.insert("userName", user.userName);
        obj.insert("password", user.password);
        user.send(obj);
        ui->pushButton_reg->setVisible(false);
    }
}

