#ifndef FRIITEM_H
#define FRIITEM_H

#include <QWidget>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>


namespace Ui {
class FriItem;
}

class FriItem : public QWidget
{
    Q_OBJECT

public:
    explicit FriItem(QJsonObject u, QWidget *parent = 0);
    ~FriItem();

private:
    Ui::FriItem *ui;
};

#endif // FRIITEM_H
