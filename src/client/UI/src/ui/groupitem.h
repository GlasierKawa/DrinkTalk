#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <QWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "src/model/user.h"
#include "chatgroform.h"

namespace Ui {
class GroupItem;
}

extern User user;

class GroupItem : public QWidget
{
    Q_OBJECT

public:
    explicit GroupItem(QJsonObject g, QWidget *parent = 0);
    ~GroupItem();


//在Qt中为QWidget创建鼠标点击处理
//https://blog.csdn.net/l285345042/article/details/8161320
signals:
    void clicked();

protected:
    virtual void mouseReleaseEvent(QMouseEvent * ev);
private slots:
    //打开聊天窗口
    void doChatClick();
    //
    void server_reply();

private:
    Ui::GroupItem *ui;

    int ownerId;
    int groupId;
};

#endif // GROUPITEM_H
