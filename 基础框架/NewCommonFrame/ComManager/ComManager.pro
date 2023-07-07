#-------------------------------------------------
#
# Project created by QtCreator 2017-11-06T15:37:46
#
#-------------------------------------------------

QT       -= core gui

TARGET = ComManager
TEMPLATE = lib

DEFINES += COMMANAGER_LIBRARY

SOURCES += ComManager.cpp

HEADERS += ComManager.h


DESTDIR = ../loadLib
LIBS += -L../lib -lCommonBus

INCLUDEPATH += . ../CommonBus/ ../include
DEPENDPATH += .


unix {
    target.path = /usr/lib
    INSTALLS += target
}
