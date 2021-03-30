#ifndef CHATGROFORM_H
#define CHATGROFORM_H

#include "moveableframelesswindow.h"
#include "src/model/user.h"
#include "memberform.h"
#include "msg.h"
#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QDateTime>
#include <QDebug>
#include <QDomElement>
#include <QScrollBar>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QWindow>

#include <QtCore/QCoreApplication>
#include <QtMultimedia/QMediaPlayer>


namespace Ui {
class ChatGroForm;
}

extern User user;

class ChatGroForm : public MoveableFramelessWindow
{
    Q_OBJECT

public:
    int id;

    std::map<int,fri> member;

    explicit ChatGroForm(int groupId, QWidget *parent = 0);
    ~ChatGroForm();

protected:
    //获得可移动窗口的控件
    QWidget*getDragnWidget();

private slots:
    //发送消息按钮
    void on_pushButton_send_clicked();
    //socket返回槽函数
    void server_reply();

    void on_pushButton_clicked();

private:
    Ui::ChatGroForm *ui;
};

#endif // CHATGROFORM_H
