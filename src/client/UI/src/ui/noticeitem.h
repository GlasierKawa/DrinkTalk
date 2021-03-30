#ifndef NOTICEITEM_H
#define NOTICEITEM_H

#include <QWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "src/model/user.h"

namespace Ui {
class NoticeItem;
}

extern User user;

class NoticeItem : public QWidget
{
    Q_OBJECT

public:
    explicit NoticeItem(QJsonObject u, QWidget *parent = 0);
    ~NoticeItem();

private slots:
    void on_pushButto_ig_clicked();

    void on_pushButton_ok_clicked();

private:
    Ui::NoticeItem *ui;
    int id = 0;
};

#endif // NOTICEITEM_H
