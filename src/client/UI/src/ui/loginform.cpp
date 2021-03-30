#include "loginform.h"
#include "ui_loginform.h"

#include "mainform.h"

#include <QGridLayout>



LoginForm::LoginForm(QWidget *parent) : MoveableFramelessWindow(parent), ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    //socket事件绑定函数
    connect(user.socket, &QTcpSocket::connected, this, &LoginForm::connect_success);
    connect(user.socket, &QTcpSocket::disconnected, this, &LoginForm::disconnect_success);
    connect(user.socket, &QTcpSocket::readyRead, this, &LoginForm::server_reply);
    //socket连接
    user.connect();
}


LoginForm::~LoginForm()
{
    delete ui;
    this->deleteLater();
}


QWidget *LoginForm::getDragnWidget()
{
    return ui->login_top_widget;
}


void LoginForm::on_pushButton_register_clicked()
{
    AddFrom*a = new AddFrom();
    a->show();
}


void LoginForm::on_pushButton_login_clicked()
{
    //user类私有成员变量 获取名称密码 发送登录
    user.userId = ui->lineEdit_un->text().toInt();
    user.password = ui->lineEdit_pass->text();

    if(user.userId == NULL)
    {
        QMessageBox::information(this,"输入提示","请输入用户名");
    }
    else if(user.password == "")
    {
        QMessageBox::information(this,"输入提示","请输入密码");
    }
    else
    {
        QJsonObject obj;
        obj.insert("cmd", "login");
        obj.insert("userId", user.userId);
        obj.insert("password", user.password);
        user.send(obj);
    }


    //////////////////////////////////////////////////////////////////////////////////////
//    MainForm *m = new MainForm;
//    m->show();
//    this->hide();
}


void LoginForm::connect_success()
{
    //连接成功消息盒
    qDebug()<<"连接服务器成功";
//    QMessageBox::information(this,"连接提示","连接服务器成功");
}


void LoginForm::disconnect_success()
{
    //断开连接消息盒
    qDebug()<<"断开服务器";
//    QMessageBox::information(this,"连接提示","断开服务器");
}


void LoginForm::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
    qDebug()<<"Loginform recive: "<<cmd;
    if (cmd == "login-reply")
    {
        client_login_handler(user.obj.value("success").toInt(), user.obj.value("user").toObject());
        user.clearobj();
    }
}



void LoginForm::client_login_handler(int success, QJsonObject myinfo)
{
    if (success == 0)
    {
        QMessageBox::warning(this, "登录提示", "用户id或密码错误");
    }
    else if(success == 1)
    {
        user.userId = myinfo.value("userId").toInt();
        user.userName = myinfo.value("userName").toString();
        user.iconStr = myinfo.value("iconStr").toString();
        user.desc = myinfo.value("desc").toString();

        fri my;
        my.userId = user.userId;
        my.userName = user.userName;
        my.iconStr = user.iconStr;
        my.desc = user.desc;
        my.open = false;
        my.online = true;
        my.msgNum = 0;
        user.friends[my.userId] = my;

        //socket readyread 不再在本页面响应
        user.socket->disconnect(SIGNAL(readyRead()));//断开readyRead所有信号槽关系
        //user.socket->disconnect(this);//断开与本对象之间的所有信号槽关系

        //创建并显示主页面
        MainForm *m = new MainForm;
        m->show();
        this->hide();
    }
    else
    {
        QMessageBox::warning(this, "登录提示", "服务器错误");
    }
}


void LoginForm::on_pushButton_findpassword_clicked()
{
//    this->deleteLater();
}

void LoginForm::on_pushButton_min_clicked()
{
    setWindowState(Qt::WindowMinimized);
}
