#ifndef SEARCHGROFORM_H
#define SEARCHGROFORM_H

#include "moveableframelesswindow.h"
#include "src/model/user.h"
#include <QWidget>

namespace Ui {
class SearchGroForm;
}

extern User user;

class SearchGroForm : public MoveableFramelessWindow
{
    Q_OBJECT

public:
    explicit SearchGroForm(int keyId, QWidget *parent = 0);
    ~SearchGroForm();

protected:
    //获得可移动窗口的控件
    QWidget*getDragnWidget();

public slots:
    void server_reply();

private slots:
    void on_pushButton_sendadd_clicked();

private:
    Ui::SearchGroForm *ui;
    int groupId;
    int ownerId;
};

#endif // SEARCHGROFORM_H
