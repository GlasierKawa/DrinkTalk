#-------------------------------------------------
#
# Project created by QtCreator 2015-02-07T19:59:32
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += widgets
QT       += xml
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QQLikeUI
TEMPLATE = app

LIBS += -lws2_32

SOURCES += main.cpp\
        mainwindow.cpp \
    src/ui/loginform.cpp \
    src/ui/collapseview.cpp \
    src/ui/collapseviewitem.cpp \
    src/ui/litteriem.cpp \
    src/ui/moveableframelesswindow.cpp \
    src/ui/mainform.cpp \
    src/model/singleton.cpp \
    src/ui/colorpickform.cpp \
    src/model/config.cpp \
    src/model/socket.cpp \
    src/ui/chatform.cpp \
    src/model/user.cpp \
    src/ui/msg.cpp \
    src/ui/addfrom.cpp \
    src/ui/searchfrom.cpp \
    src/ui/friitem.cpp \
    src/ui/noticelist.cpp \
    src/ui/noticeitem.cpp \
    src/ui/creategroup.cpp \
    src/ui/grouplist.cpp \
    src/ui/groupitem.cpp \
    src/ui/searchgroform.cpp \
    src/ui/chatgroform.cpp \
    src/ui/memberform.cpp

HEADERS  += mainwindow.h \
    src/ui/loginform.h \
    src/ui/collapseview.h \
    src/ui/collapseviewitem.h \
    src/ui/litteriem.h \
    src/ui/moveableframelesswindow.h \
    src/ui/mainform.h \
    src/model/singleton.h \
    src/ui/colorpickform.h \
    src/model/config.h \
    src/model/socket.h \
    src/ui/chatform.h \
    src/model/user.h \
    src/ui/msg.h \
    src/ui/addfrom.h \
    src/ui/searchfrom.h \
    src/ui/friitem.h \
    src/ui/noticelist.h \
    src/ui/noticeitem.h \
    src/ui/creategroup.h \
    src/ui/grouplist.h \
    src/ui/groupitem.h \
    src/ui/searchgroform.h \
    src/ui/chatgroform.h \
    src/ui/memberform.h

FORMS    += mainwindow.ui \
    src/ui/loginform.ui \
    src/ui/collapseview.ui \
    src/ui/collapseviewitem.ui \
    src/ui/litteriem.ui \
    src/ui/mainform.ui \
    src/ui/colorpickform.ui \
    src/ui/chatform.ui \
    src/ui/msg.ui \
    src/ui/addfrom.ui \
    src/ui/searchfrom.ui \
    src/ui/friitem.ui \
    src/ui/noticelist.ui \
    src/ui/noticeitem.ui \
    src/ui/creategroup.ui \
    src/ui/grouplist.ui \
    src/ui/groupitem.ui \
    src/ui/searchgroform.ui \
    src/ui/chatgroform.ui \
    src/ui/memberform.ui

RESOURCES += \
    rs/rs.qrc


