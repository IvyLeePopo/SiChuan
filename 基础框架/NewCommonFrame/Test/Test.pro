QT += core
QT -= gui

CONFIG += c++11

TARGET = Test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp


DESTDIR = ../bin

LIBS += -L../lib -lCommonBus -lCommonFrameMain
INCLUDEPATH += . ../CommonBus/ ../include
DEPENDPATH += .

INCLUDEPATH += ../CommonFrameMain
DEPENDPATH += .

DISTFILES += \
    ../output/bin/CommonBus.dll \
    ../output/bin/CommonFrameMain.dll \
    ../output/bin/Test.exe \
    ../output/lib/libCommonBus.a \
    ../output/lib/libCommonFrameMain.a \
    ../output/lib/CommonBus.dll \
    ../output/lib/CommonFrameMain.dll \
    ../output/loadLib/libComManager.a \
    ../output/loadLib/ComManager.dll


