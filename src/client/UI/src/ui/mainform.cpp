#include "mainform.h"
#include "ui_mainform.h"

#include "collapseview.h"
#include "colorpickform.h"


#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>


/*
background-color: rgba(255, 255, 255, 0);
color: rgb(255, 255, 255);
font: 87 9pt "Segoe UI Black";
*/


//主串口与屏幕边缘的距离
int MAIN_FORM_OADDING =100;


MainForm::MainForm(QWidget *parent) : MoveableFramelessWindow(parent), ui(new Ui::MainForm)
{
    ui->setupUi(this);

    //关联切换皮肤颜色按钮事件
    connect(ui->pushButton_skin,SIGNAL(clicked()),this,SLOT(doChangeColor()));
    //socket返回事件绑定函数
    connect(user.socket, &QTcpSocket::readyRead, this, &MainForm::server_reply);

    ui->label_id->setNum(user.userId);
    ui->label_nick->setText(user.userName);
    ui->lineEdit->setText(user.desc);

    //这三个addWidget循序不可更改
    ui->page_5->layout()->addWidget(new GroupList());
    ui->page_6->layout()->addWidget(new NoticeList());
    ui->page_4->layout()->addWidget(new CollapseView());



    //获取屏幕
    QDesktopWidget* desktopWidget = QApplication::desktop();
    //得到屏幕宽度
    int screenWidth = desktopWidget->width();
    //转移到右边
    this->move(screenWidth-MAIN_FORM_OADDING-this->width(), MAIN_FORM_OADDING);

    //关闭时执行析构函数
    setAttribute(Qt::WA_DeleteOnClose);
}


MainForm::~MainForm()
{
    QJsonObject obj;
    obj.insert("cmd", "offline");
    obj.insert("userId", user.userId);
    user.send(obj);

    user.disconnect();
    delete ui;
    this->deleteLater();
}


void MainForm::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
    qDebug()<<"Mainform recive: "<<cmd;
    if (cmd == "NONE")
    {
        user.clearobj();
    }
    else if(cmd == "sendGroupMessage-reply")
    {
        user.clearobj();
    }

}


void MainForm::doChangeColor()
{
    //MY_COLOR_PICK_FORM 为 ColorPickForm::getInstance()
    //单例模式，一般用于比较大，复杂的对象，只初始化一次，
    //应该设置private的构造函数，使得不能用new来实例化对象，
    //只能调用getInstance方法来得到对象，而getInstance保证了每次调用都返回相同的对象。
    // https://blog.csdn.net/mccoy39082/article/details/82050378

    //显示窗口
    MY_COLOR_PICK_FORM.show();
    //窗口置前
    MY_COLOR_PICK_FORM.raise();
    //使Widge获得焦点
    MY_COLOR_PICK_FORM.setFocus();

    ColorPickForm*f=&(MY_COLOR_PICK_FORM);
    //颜色改变信号 绑定到 颜色改变函数
    connect(f,SIGNAL(themColorChanged(QString)),this,SLOT(onThemeColorChange(QString)));

}


void MainForm::onThemeColorChange(QString colorStr)
{
    //组合成qcc样式表
    QString style=QString("#%1{background-color:%2;}").arg(ui->centralwidget->objectName(),colorStr);
    //定义样式
    ui->centralwidget->setStyleSheet(style);
}


QWidget *MainForm::getDragnWidget()
{
    return ui->main_top;
}


//切换 聊天 群聊
// https://blog.csdn.net/u011731378/article/details/79975782
void MainForm::on_pushButton_p_clicked()
{
    ui->center_stack->setCurrentWidget(ui->page_4);
}

void MainForm::on_pushButton_q_clicked()
{
    ui->center_stack->setCurrentWidget(ui->page_5);
}

void MainForm::on_pushButton_t_clicked()
{
    ui->center_stack->setCurrentWidget(ui->page_6);
}


void MainForm::on_pushButton_su_clicked()
{
    int keyId = ui->search->text().toInt();
    SearchFrom*s = new SearchFrom(keyId);
    s->show();
}


void MainForm::on_pushButton_sg_clicked()
{
    int keyId = ui->search->text().toInt();
    SearchGroForm*s = new SearchGroForm(keyId);
    s->show();
}

void MainForm::on_pushButton_min_clicked()
{
    //最小化
    // https://blog.csdn.net/sunbo94/article/details/79673387
    setWindowState(Qt::WindowMinimized);
}

void MainForm::on_pushButton_cg_clicked()
{
    CreateGroup*c = new CreateGroup;
    c->show();
}



