#include "mainwindow.h"
#include "src/ui/loginform.h"
#include "src/ui/mainform.h"
#include "src/model/user.h"

#include <QApplication>

#include <QDebug>
#include <QJsonArray>

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTime>


#include <QtCore/QCoreApplication>
#include <QtMultimedia/QMediaPlayer>





//全局变量 保存socket username password
User user;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QMediaPlayer * player = new QMediaPlayer;
    player->setMedia(QUrl("qrc:/media/radio/fla2.mp3"));
    player->setVolume(5);
    player->play();


    //    MainWindow w;
    //    w.show();
    //    SlideWindow l;
    //    MainForm l;

    LoginForm l;
    l.show();



    return a.exec();
}







//    QTime t;
//    t.start();
//    while(t.elapsed()<3000) QCoreApplication::processEvents();




