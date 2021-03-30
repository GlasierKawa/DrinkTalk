#ifndef ADDFROM_H
#define ADDFROM_H

#include "moveableframelesswindow.h"
#include "src/model/user.h"
#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QDomElement>

namespace Ui {
class AddFrom;
}

extern User user;

class AddFrom : public MoveableFramelessWindow
{
    Q_OBJECT

public:
    explicit AddFrom(QWidget *parent = 0);
    ~AddFrom();

    void server_reply();

protected:
    //获得可移动窗口的控件
    QWidget*getDragnWidget();

private slots:
    void on_pushButton_reg_clicked();

private:
    Ui::AddFrom *ui;
};

#endif // ADDFROM_H
