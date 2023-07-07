QT += core
QT -= gui
QT += xml
CONFIG += c++11

TARGET = Test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp


DESTDIR = ../bin

LIBS += -L../lib -lCommonFrameMain
LIBS += -L../lib -lCommonBus
#LIBS += -L../loadLib -lUpdateManager

INCLUDEPATH += . ../CommonBus/ ../include
DEPENDPATH += .

INCLUDEPATH += ../CommonFrameMain
DEPENDPATH += .

INCLUDEPATH += . ../UpdateManager/ ../include

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


