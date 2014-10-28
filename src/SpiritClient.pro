#-------------------------------------------------
#
# Project created by QtCreator 2013-11-19T21:54:00
#
#-------------------------------------------------

QT       += core gui script network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SpiritClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mousewidget.cpp \
    configworker.cpp \
    sutil.cpp \
    networker.cpp \
    regworker.cpp

HEADERS  += mainwindow.h \
    mousewidget.h \
    configworker.h \
    sutil.h \
    networker.h \
    regworker.h

FORMS    += mainwindow.ui

RC_FILE += SpiritClient.rc

RESOURCES += \
    SpiritClient.qrc
