#ifndef LITTERIEM_H
#define LITTERIEM_H

#include <QWidget>
#include "src/model/user.h"
#include "chatform.h"
#include <QPixmap>
#include <QtCore/QCoreApplication>
#include <QtMultimedia/QMediaPlayer>

namespace Ui {
class LitterIem;
}

extern User user;

class LitterIem : public QWidget
{
    Q_OBJECT

public:

    int id;

    //构造
    explicit LitterIem(int, QWidget *parent = 0);
    //析构
    ~LitterIem();


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
    Ui::LitterIem *ui;
};

#endif // LITTERIEM_H
