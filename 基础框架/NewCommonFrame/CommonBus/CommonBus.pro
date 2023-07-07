#-------------------------------------------------
#
# Project created by QtCreator 2017-11-06T09:44:18
#
#-------------------------------------------------

QT       -= core gui

TARGET = CommonBus
TEMPLATE = lib

DEFINES += COMMONBUS_LIBRARY

SOURCES += CommonBus.cpp \
    CommonBusSource.cpp

HEADERS += CommonBus.h\
    CommonBusSource.h


DESTDIR = ../lib

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    ReadMe.txt

OTHER_FILES += \
    ReadMe.txt
