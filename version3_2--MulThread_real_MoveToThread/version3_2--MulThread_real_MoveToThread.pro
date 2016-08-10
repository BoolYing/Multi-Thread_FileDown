#-------------------------------------------------
#
# Project created by QtCreator 2016-08-04T13:30:28
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = version3_2--MulThread_real_MoveToThread
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    download.cpp \
    downloadcontrol.cpp

HEADERS  += mainwindow.h \
    download.h \
    downloadcontrol.h

FORMS    += mainwindow.ui
