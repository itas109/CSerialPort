#-------------------------------------------------
#
# Project created by QtCreator 2019-02-27T21:13:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CommQT
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#DEFINES += QT_NO_EMIT
DEFINES += _UNICODE

include($$PWD/commqt.pri)

win32:LIBS += -lsetupapi
unix:!macx:LIBS += -lpthread
macx {
    LIBS += -framework IOKit
    LIBS += -framework Foundation
}
