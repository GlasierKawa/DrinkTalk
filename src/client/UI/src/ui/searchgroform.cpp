#include "searchgroform.h"
#include "ui_searchgroform.h"

SearchGroForm::SearchGroForm(int keyId, QWidget *parent) : MoveableFramelessWindow(parent), ui(new Ui::SearchGroForm)
{
    ui->setupUi(this);
    //设置阴影
    setShadowEffect(ui->widget_bg);
    //图片适应大小
    ui->label_icon->setScaledContents(true);

    connect(user.socket, &QTcpSocket::readyRead, this, &SearchGroForm::server_reply);

    //查找用户请求
    QJsonObject obj;
    obj.insert("cmd", "searchGroupById");
    obj.insert("groupId", keyId);
    obj.insert("sendId", user.userId);
    user.send(obj);

    //关闭时执行析构函数
    setAttribute(Qt::WA_DeleteOnClose);
}


SearchGroForm::~SearchGroForm()
{
    delete ui;
    this->deleteLater();
}

QWidget *SearchGroForm::getDragnWidget()
{
    return ui->widget_h;
}


void SearchGroForm::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
    qDebug()<<"SearchGroForm recive: "<<cmd;
    if (cmd == "searchGroupById-reply")
    {
        if(user.obj.value("exist").toBool() == true)
        {
            QJsonObject g = user.obj.value("group").toObject();
            QPixmap p(":/media/qun/media/qun/3.png");
            ui->label_icon->setPixmap(p);
            ui->label_name->setText(g.value("groupName").toString());
            ui->label_owner->setNum(g.value("ownerId").toInt());
            ui->label_id->setNum(g.value("groupId").toInt());
            groupId = g.value("groupId").toInt();
            ownerId = g.value("ownerId").toInt();

            if(user.obj.value("already").toBool() == true)
            {
                ui->pushButton_sendadd->setText("已添加");
                ui->pushButton_sendadd->setEnabled(false);
                ui->pushButton_sendadd->setStyleSheet("backgroud-color:rgb(150,150,150);");
            }
        }
        else
        {
            ui->pushButton_sendadd->setVisible(false);
        }
        user.clearobj();
    }
}


void SearchGroForm::on_pushButton_sendadd_clicked()
{
    QJsonObject obj;
    obj.insert("cmd", "requestAddGroup");
    obj.insert("userId", user.userId);
    obj.insert("groupId", groupId);
    obj.insert("ownerId", ownerId);
    user.send(obj);

    ui->pushButton_sendadd->setText("已添加");
    ui->pushButton_sendadd->setEnabled(false);
    ui->pushButton_sendadd->setStyleSheet("backgroud-color:rgb(150,150,150);");
}


