#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include "moveableframelesswindow.h"
#include "src/model/user.h"
#include <QGraphicsDropShadowEffect>
#include <QWidget>


namespace Ui {
class CreateGroup;
}

extern User user;

class CreateGroup : public MoveableFramelessWindow
{
    Q_OBJECT

public:
    explicit CreateGroup(QWidget *parent = 0);
    ~CreateGroup();

protected:
    //获得可移动窗口的控件
    QWidget*getDragnWidget();

private slots:
    void on_pushButton_create_clicked();

private:
    Ui::CreateGroup *ui;
};

#endif // CREATEGROUP_H
