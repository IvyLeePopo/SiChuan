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

SOURCES += CommonFrameMain.cpp \
    json/json_writer.cpp \
    json/json_valueiterator.inl \
    json/json_value.cpp \
    json/json_reader.cpp \
    json/json_internalmap.inl \
    json/json_internalarray.inl \
    FrameConfigFileOperate.cpp

HEADERS += CommonFrameMain.h \
    ExternalLibApi.h \
    json/writer.h \
    json/value.h \
    json/reader.h \
    json/json_batchallocator.h \
    json/json.h \
    json/forwards.h \
    json/features.h \
    json/config.h \
    json/autolink.h \
    FrameConfigFileOperate.h

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

OTHER_FILES += \
    json/sconscript
