#-------------------------------------------------
#
# Project created by QtCreator 2017-08-11T13:54:39
#
#-------------------------------------------------

QT       -= gui
QT       +=  xml sql  network
TARGET = TWSDNetPay_Dll
TEMPLATE = lib

DEFINES += TWSDNETPAY_LIBRARY

DESTDIR = ../loadLib

# -Ldir: 指定库搜索路径
LIBS += -L../lib  -lCommonBus
INCLUDEPATH += . ../CommonBus/ ../include
DEPENDPATH += .

#LINUX平台下面
unix:!macx: {

# -l library : 指定所需要的额外库
 LIBS  += -lrt

 }



SOURCES += twsdnetpay.cpp \
    jsoncpp/json_internalarray.inl \
    jsoncpp/json_internalmap.inl \
    jsoncpp/json_reader.cpp \
    jsoncpp/json_value.cpp \
    jsoncpp/json_valueiterator.inl \
    jsoncpp/json_writer.cpp \
    cdvjsonbase.cpp \
    cdvjsondebit.cpp \
    cdvjsondebitcancel.cpp \
    cdvjsonparamaccount.cpp \
    cdvjsonparamdebitcancel.cpp \
    cdvjsonparamdebitmoney.cpp \
    cdvjsonparamdebitresult.cpp \
    cdvjsonparammmi.cpp \
    c_logtxt.cpp \
    cthreadmanage.cpp \
    cthreadbase.cpp \
    chtsddevicedll.cpp \
    cthreaddevice.cpp \
    cjsonoperationif.cpp \
    cdvxmldebitcancel.cpp \
    cdvxmldebitresult.cpp \
    cdvxmlbase.cpp \
    cjsonparamoperate.cpp \
    cdvxmldebitparamcontext.cpp \
    cdvxmlmmi.cpp \
    cdvxmlaccountcontext.cpp \
    cjsondebitoperate.cpp \
    UpLoad.cpp \
    MD5.cpp \
    UpLoadLog.cpp \
    netpaydll.cpp


HEADERS += twsdnetpay.h\
        twsdnetpay_global.h \
    jsoncpp/autolink.h \
    jsoncpp/value.h \
    jsoncpp/writer.h \
    jsoncpp/config.h \
    jsoncpp/features.h \
    jsoncpp/forwards.h \
    jsoncpp/json.h \
    jsoncpp/json_batchallocator.h \
    jsoncpp/reader.h \
    basedefine.h \
    cdvjsonbase.h \
    cdvjsondebit.h \
    cdvjsondebitcancel.h \
    cdvjsonparamaccount.h \
    cdvjsonparamdebitcancel.h \
    cdvjsonparamdebitmoney.h \
    cdvjsonparamdebitresult.h \
    cdvjsonparammmi.h \
    c_logtxt.h \
    cthreadmanage.h \
    cthreadbase.h \
    chtsddevicedll.h \
    cthreaddevice.h \
    cjsonoperationif.h \
    cdvxmldebitcancel.h \
    cdvxmldebitresult.h \
    cdvxmlbase.h \
    cjsonparamoperate.h \
    cdvxmldebitparamcontext.h \
    cdvxmlmmi.h \
    cdvxmlaccountcontext.h \
    cjsondebitoperate.h \
    UpLoad.h \
    MD5.h \
    UpLoadLog.h \
    netpaydll.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES += \
    jsoncpp/sconscript \
    TWSDNetPay.pro.user











