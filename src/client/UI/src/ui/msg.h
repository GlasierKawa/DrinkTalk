#ifndef MSG_H
#define MSG_H

#include <QWidget>
#include <QtCore/QCoreApplication>
#include <QtMultimedia/QMediaPlayer>

#include "src/model/user.h"

extern User user;

namespace Ui {
class Msg;
}

class Msg : public QWidget
{
    Q_OBJECT

public:
    int id;

    explicit Msg(fri u,
                 QString msg,
                 QString time,
                 QWidget *parent = 0);
    ~Msg();

private:
    Ui::Msg *ui;
};

#endif // MSG_H
