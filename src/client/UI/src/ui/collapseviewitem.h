#ifndef COLLAPSEVIEWITEM_H
#define COLLAPSEVIEWITEM_H

#include <QWidget>
#include <QList>
#include <QHostAddress>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTime>
#include <QPixmap>

#include "src/model/user.h"
#include "litteriem.h"



extern User user;

namespace Ui {
class CollapseViewItem;
}

class CollapseViewItem : public QWidget
{
    Q_OBJECT

public:
    //构造
    explicit CollapseViewItem(QString titleText,QWidget *parent = 0);
    //析构
    ~CollapseViewItem();
    //获取展示好友列表
    void showFriendList(QJsonArray friends);
    //添加新好友
    void addFriendItem(QJsonObject f);

private slots:
    //socket返回槽函数
    void server_reply();
    //点击按钮进行项目折叠
    void onCollapseButClick();

private:
    Ui::CollapseViewItem *ui;

};

#endif // COLLAPSEVIEWITEM_H
