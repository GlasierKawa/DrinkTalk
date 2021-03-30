#ifndef CHATFORM_H
#define CHATFORM_H

#include "moveableframelesswindow.h"
#include "src/model/user.h"
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
class ChatForm;
}


extern User user;


class ChatForm : public MoveableFramelessWindow
{
    Q_OBJECT

public:
    int id;

    explicit ChatForm(int userId, QWidget *parent = 0);
    ~ChatForm();

protected:
    //获得可移动窗口的控件
    QWidget*getDragnWidget();

private slots:
    //发送消息按钮
    void on_pushButton_send_clicked();
    //socket返回槽函数
    void server_reply();

    void on_pushButton_min_clicked();

private:
    Ui::ChatForm *ui;
};

#endif // CHATFORM_H
