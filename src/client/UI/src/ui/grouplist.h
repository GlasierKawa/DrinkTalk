#ifndef GROUPLIST_H
#define GROUPLIST_H

#include "groupitem.h"
#include "src/model/user.h"

#include <QWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTime>

#include <QtCore/QCoreApplication>
#include <QtMultimedia/QMediaPlayer>

namespace Ui {
class GroupList;
}

extern User user;

class GroupList : public QWidget
{
    Q_OBJECT

public:
    explicit GroupList(QWidget *parent = 0);
    ~GroupList();

private slots:
    //socket返回槽函数
    void server_reply();

private:
    Ui::GroupList *ui;
};

#endif // GROUPLIST_H
