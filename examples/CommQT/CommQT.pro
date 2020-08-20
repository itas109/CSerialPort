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

#qmake -query => QMAKE_SPEC(msvc):win32-msvc2013 QMAKE_SPEC(mingw):win32-g++
#about windows reg
win32-msvc*:LIBS += advapi32.lib
#about windows setupapi
win32-msvc*:LIBS += setupapi.lib

win32-g++:LIBS += libsetupapi

