#-------------------------------------------------
#
# Project created by QtCreator 2017-11-14T17:52:15
#
#-------------------------------------------------

QT       -= gui
QT      += xml
QT      += network
TARGET = UpdateManager
TEMPLATE = lib

DEFINES += UPDATEMANAGER_LIBRARY

DEFINES += ZIP_STD
SOURCES += UpdateManager.cpp \
    json/json_internalarray.inl \
    json/json_internalmap.inl \
    json/json_reader.cpp \
    json/json_value.cpp \
    json/json_valueiterator.inl \
    json/json_writer.cpp \
    FileManager/FileManager.cpp \
    FileManager/ReadVersionXml.cpp \
    UpdateConfirm/UpdateConfirm.cpp \
    UpdateDownLoad/UpdateDownLoad.cpp \
    UpdateHttp/UpdateFromHttpManager.cpp \
    UpdateQuery/UpdateQuery.cpp \
    UpdateHttp/TestResponse.cpp \
    ZipManager/zip.cpp \
    ZipManager/unzip.cpp \
    UpdateCommon.cpp

HEADERS += UpdateManager.h \
    FileManager.h \
    json/autolink.h \
    json/config.h \
    json/features.h \
    json/forwards.h \
    json/json.h \
    json/json_batchallocator.h \
    json/reader.h \
    json/value.h \
    json/writer.h \
    UpdateManagerGlobal.h \
    ReadVersionXml.h \
    UpdateFromHttpManager.h \
    UpdateQuery.h \
    UpdateDownLoad.h \
    FileManager/FileManager.h \
    FileManager/ReadVersionXml.h \
    UpdateConfirm/UpdateConfirm.h \
    UpdateDownLoad/UpdateDownLoad.h \
    UpdateHttp/UpdateFromHttpManager.h \
    UpdateQuery/UpdateQuery.h \
    UpdateHttp/UpdateHttpTimeOut.h \
    UpdateHttp/TestResponse.h \
    ZipManager/zip.h \
    ZipManager/unzip.h \
    UpdateCommon.h


DESTDIR = ../loadLib
LIBS += -L../lib -lCommonBus

INCLUDEPATH += . ../CommonBus/ ../include
DEPENDPATH += .

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    json/sconscript
