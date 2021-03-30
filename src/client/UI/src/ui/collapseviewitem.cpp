#include "collapseviewitem.h"
#include "ui_collapseviewitem.h"



int MAX_PERSON_PIC_NUM=9;

CollapseViewItem::CollapseViewItem(QString titleText, QWidget *parent) : QWidget(parent), ui(new Ui::CollapseViewItem)
{
    ui->setupUi(this);
    //自定义属性selected
    ui->label->setProperty("selected",!ui->item_con->isVisible());

    //显示分组内好友按钮信号槽
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onCollapseButClick()));
    //socket返回事件绑定函数
    connect(user.socket, &QTcpSocket::readyRead, this, &CollapseViewItem::server_reply);

    //显示标题内容
    ui->pushButton->setText(titleText);

    //发送获取好友列表请求
    QJsonObject obj;
    obj.insert("cmd", "getFriendList");
    obj.insert("userId", user.userId);
    user.send(obj);

    setAttribute(Qt::WA_DeleteOnClose);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    QJsonArray f;
//    QJsonObject o;
//    o.insert("userId",123);
//    o.insert("userName","阿萨德");
//    o.insert("iconStr",":/media/person/media/person/1.jpg");
//    o.insert("desc","Hello QT");
//    o.insert("online",true);
//    f.append(o);
//    o["userId"]=124;
//    o["userName"]="在现场";
//    o["iconStr"]=":/media/person/media/person/2.jpg";
//    f.append(o);
//    o["userId"]=125;
//    o["userName"]="阿斯顿";
//    o["iconStr"]=":/media/person/media/person/3.jpg";
//    f.append(o);
//    showFriendList(f);
}


CollapseViewItem::~CollapseViewItem()
{
    delete ui;
}


void CollapseViewItem::showFriendList(QJsonArray friends)
{
    fri frid;
    LitterIem* lpt;
    for(QJsonArray::iterator item = friends.begin(); item != friends.end(); item++)
    {
        frid.userId = (*item).toObject().value("userId").toInt();
        frid.userName = (*item).toObject().value("userName").toString();
        frid.iconStr = (*item).toObject().value("iconStr").toString();// ":/media/person/media/person/%1.jpg"
        frid.desc = (*item).toObject().value("desc").toString();
        frid.online = (*item).toObject().value("online").toBool();
        frid.msgNum = (*item).toObject().value("msgNum").toInt();
        frid.open = false;

        user.friends[frid.userId] = frid;
        lpt = new LitterIem(frid.userId, this);
        ui->item_con->layout()->addWidget(lpt);
    }
}


void CollapseViewItem::addFriendItem(QJsonObject f)
{
    fri frid;
    LitterIem* lpt;

    frid.userId = f.value("userId").toInt();
    frid.userName = f.value("userName").toString();
    frid.iconStr = f.value("iconStr").toString();// ":/media/person/media/person/%1.jpg"
    frid.desc = f.value("desc").toString();
    frid.online = f.value("online").toBool();
    frid.msgNum = 0;
    frid.open = false;

    user.friends[frid.userId] = frid;
    lpt = new LitterIem(frid.userId, this);
    ui->item_con->layout()->addWidget(lpt);
}


void CollapseViewItem::server_reply()
{
    user.receive();
    //获取json的cmd数据值
    QString cmd = user.obj.value("cmd").toString();
    qDebug()<<"Collapseviewitem receive: "<<cmd;
    if (cmd == "getFriendList-reply")
    {
        showFriendList(user.obj.value("user").toArray());
        user.clearobj();

        //请求获取通知列表信息
        QJsonObject obj;
        obj.insert("cmd", "readAddFriendNotification");
        obj.insert("userId", user.userId);
        user.send(obj);
    }
    if (cmd == "addUserToFriendList")
    {
        addFriendItem(user.obj.value("user").toObject());
        user.clearobj();
    }
}


void CollapseViewItem::onCollapseButClick()
{
    //设置为相反的显示状态
    ui->item_con->setVisible(!ui->item_con->isVisible());
    //自定义属性
    ui->label->setProperty("selected",ui->item_con->isVisible());

    //将图像资源载入对象img
    QPixmap img_r(":/sys/arrow_r.png");
    QPixmap img_d(":/sys/arrow_d.png");
    if(ui->item_con->isVisible())
    {
        ui->label->setPixmap(img_d);
    }
    else
    {
        ui->label->setPixmap(img_r);
    }

    //更新
    ui->label->update();
}
