#ifndef LOGINFORM_H
#define LOGINFORM_H

#include "moveableframelesswindow.h"
#include "src/model/user.h"
#include "addfrom.h"

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QWindow>

#include <QDebug>
#include <QSound>



namespace Ui {
    class LoginForm;
}


//全局变量
extern User user;


class LoginForm :public MoveableFramelessWindow
{
    Q_OBJECT

public:
    //构造
    explicit LoginForm(QWidget *parent = 0);
    //析构
    ~LoginForm();

protected:
    //获得可移动窗口的控件
    QWidget*getDragnWidget();

private slots:
    //socket连接成功槽函数
    void connect_success();
    //socket断开槽函数
    void disconnect_success();
    //socket返回槽函数
    void server_reply();

    //登录clicked槽函数
    void on_pushButton_min_clicked();

    void on_pushButton_login_clicked();
    //注册clicked槽函数
    void on_pushButton_register_clicked();

    void on_pushButton_findpassword_clicked();

private:
    //登录返回处理
    void client_login_handler(int, QJsonObject);

    Ui::LoginForm *ui;
};

#endif // LOGINFORM_H
