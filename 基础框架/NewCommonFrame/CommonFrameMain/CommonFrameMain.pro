#-------------------------------------------------
#
# Project created by QtCreator 2017-11-06T10:25:01
#
#-------------------------------------------------

QT       -= core gui
QT       += core
TARGET = CommonFrameMain
TEMPLATE = lib

DEFINES += COMMONFRAMEMAIN_LIBRARY

SOURCES += CommonFrameMain.cpp

HEADERS += CommonFrameMain.h \
    ExternalLibApi.h

DESTDIR = ../lib

LIBS += -L../lib  -lCommonBus

INCLUDEPATH += . ../CommonBus/ ../include
DEPENDPATH += .

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    ReadMe.txt
