#ifndef MAINFORM_H
#define MAINFORM_H

#include "moveableframelesswindow.h"
#include "src/model/user.h"
#include "grouplist.h"
#include "searchfrom.h"
#include "searchgroform.h"
#include "noticelist.h"
#include "creategroup.h"

#include "addfrom.h"
#include <QWidget>
#include <QDomElement>
#include <QSound>
#include <QWindow>


#include <QtCore/QCoreApplication>
#include <QtMultimedia/QMediaPlayer>





extern User user;


namespace Ui {
    class MainForm;
}

class MainForm : public MoveableFramelessWindow
{
    Q_OBJECT

public:
    //构造
    explicit MainForm(QWidget *parent = 0);
    //析构
    ~MainForm();

private slots:
    //显示 改变主题颜色窗口  绑定 改变颜色信号 由pushButton_skin触发
    void doChangeColor();
    //改变qcc主题颜色
    void onThemeColorChange(QString colorStr);

    //切换
    void on_pushButton_p_clicked();
    void on_pushButton_q_clicked();
    void on_pushButton_t_clicked();

    //socket返回槽函数
    void server_reply();

    void on_pushButton_su_clicked();

    void on_pushButton_min_clicked();

    void on_pushButton_cg_clicked();



    void on_pushButton_sg_clicked();

protected:
    //获得可移动窗口的控件
    QWidget*getDragnWidget();

private:
    Ui::MainForm *ui;
};

#endif // MAINFORM_H
