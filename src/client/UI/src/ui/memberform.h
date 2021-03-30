#ifndef MEMBERFORM_H
#define MEMBERFORM_H

#include <QWidget>
#include <QPixmap>
#include "src/model/user.h"


namespace Ui {
class MemberForm;
}

extern User user;

class MemberForm : public QWidget
{
    Q_OBJECT

public:
    explicit MemberForm(fri m, QWidget *parent = 0);
    ~MemberForm();

private:
    Ui::MemberForm *ui;
};

#endif // MEMBERFORM_H
