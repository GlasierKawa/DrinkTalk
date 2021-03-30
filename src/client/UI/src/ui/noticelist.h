#ifndef NOTICELIST_H
#define NOTICELIST_H

#include "noticeitem.h"
#include "src/model/user.h"

#include <QWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTime>

#include <QtCore/QCoreApplication>
#include <QtMultimedia/QMediaPlayer>


namespace Ui {
class NoticeList;
}

extern User user;

class NoticeList : public QWidget
{
    Q_OBJECT

public:
    explicit NoticeList(QWidget *parent = 0);
    ~NoticeList();

    void addNoticeList(QJsonArray notices);

private slots:
    //socket返回槽函数
    void server_reply();

private:
    Ui::NoticeList *ui;
};

#endif // NOTICELIST_H
