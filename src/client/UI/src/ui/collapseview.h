#ifndef COLLAPSEVIEW_H
#define COLLAPSEVIEW_H

#include <QWidget>
#include "src/model/user.h"


extern User user;


namespace Ui {
class CollapseView;
}

class CollapseView : public QWidget
{
    Q_OBJECT

public:
    //构造
    explicit CollapseView(QWidget *parent = 0);
    //析构
    ~CollapseView();

private slots:
    //
    void butClick();

private:
    Ui::CollapseView *ui;
};

#endif // COLLAPSEVIEW_H
