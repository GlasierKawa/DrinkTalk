#ifndef SEARCHFROM_H
#define SEARCHFROM_H

#include "moveableframelesswindow.h"
#include "src/model/user.h"

#include <QWidget>

extern User user;

namespace Ui {
class SearchFrom;
}

class SearchFrom : public MoveableFramelessWindow
{
    Q_OBJECT

public:
    explicit SearchFrom(int keyId, QWidget *parent = 0);
    ~SearchFrom();


public slots:
    void server_reply();


protected:
    //获得可移动窗口的控件
    QWidget*getDragnWidget();



private slots:
    void on_pushButton_sendadd_clicked();

private:
    Ui::SearchFrom *ui;

    int id = 0;
};

#endif // SEARCHFROM_H
