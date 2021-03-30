#include "friitem.h"
#include "ui_friitem.h"

FriItem::FriItem(QJsonObject u, QWidget *parent) : QWidget(parent), ui(new Ui::FriItem)
{
    ui->setupUi(this);

    QPixmap p(u.value("iconStr").toString());
    ui->label_icon->setPixmap(p);
    ui->label_name->setText(u.value("userName").toString());
    ui->label_desc->setText(u.value("desc").toString());
    ui->label_id->setNum(u.value("userId").toInt());
}

FriItem::~FriItem()
{
    delete ui;
}
