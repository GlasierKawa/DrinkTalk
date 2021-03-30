#include "memberform.h"
#include "ui_memberform.h"

MemberForm::MemberForm(fri m, QWidget *parent) : QWidget(parent), ui(new Ui::MemberForm)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    QPixmap p(m.iconStr);
    ui->label_pic->setPixmap(p);
    ui->label_name->setText(m.userName);
    ui->label_id->setNum(m.userId);
}

MemberForm::~MemberForm()
{
    delete ui;
}
