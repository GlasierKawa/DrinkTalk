#include "creategroup.h"
#include "ui_creategroup.h"



CreateGroup::CreateGroup(QWidget *parent) : MoveableFramelessWindow(parent), ui(new Ui::CreateGroup)
{
    ui->setupUi(this);

    setShadowEffect(ui->widget_bg);
    setAttribute(Qt::WA_DeleteOnClose);

}

CreateGroup::~CreateGroup()
{
    delete ui;
    this->deleteLater();
}

QWidget *CreateGroup::getDragnWidget()
{
    return ui->widget_h;
}

void CreateGroup::on_pushButton_create_clicked()
{
    QString gn = ui->lineEdit_gn->text();
    QJsonObject obj;
    obj.insert("cmd", "createGroup");
    obj.insert("userId", user.userId);
    obj.insert("groupName", gn);
    user.send(obj);

    ui->pushButton_create->setText("创建成功");
    ui->pushButton_create->setEnabled(false);
    ui->pushButton_create->setStyleSheet("backgroud-color:rgb(150,150,150);");

}
