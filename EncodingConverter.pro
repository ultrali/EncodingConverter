#-------------------------------------------------
#
# Project created by QtCreator 2017-06-24T12:24:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EncodingConverter
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dialog.cpp

HEADERS += \
        mainwindow.h \
    dialog.h

FORMS += \
        mainwindow.ui \
    dialog.ui

SUBDIRS += \
    EncodingConverter.pro

DISTFILES += \
    icon.png \
    icon.ico \
    rc.rc

RC_FILE = rc.rc

#win32 {
#    DEFINES += WIN32
#    INCLUDEPATH += D:/Developer/uchardet-master/src
#    LIBS += -LD:/Developer/uchardet-master/Libs -luchardet
#    #LIBS += D:/Framework/uchardet-master/Libs/libuchardet.a
#}

unix {
    # apt-get install uchardet libuchardet-dev
    LIBS += -luchardet
}
