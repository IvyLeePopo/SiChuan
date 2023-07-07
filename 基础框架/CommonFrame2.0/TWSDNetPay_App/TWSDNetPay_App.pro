#-------------------------------------------------
#
# Project created by QtCreator 2017-02-07T16:31:52
#
#-------------------------------------------------

QT       -= gui
QT       += core xml sql
QT       += network



TARGET = TWSDNetPay_App
CONFIG += console
CONFIG -= app_bundle
DEFINES += PAYSYSTEM_LIBRARY

TEMPLATE = lib

DEFINES += TWSDNETPAY_APP_LIBRARY

DESTDIR = ../loadLib
LIBS += -L../lib -lCommonBus

INCLUDEPATH += . ../CommonBus/ ../include
DEPENDPATH += .

SOURCES += \
    TWSDNetPay_Dll/cdvcallbackthread.cpp \
    TWSDNetPay_Dll/cdvjsonbase.cpp \
    TWSDNetPay_Dll/cdvjsondebit.cpp \
    TWSDNetPay_Dll/cdvjsondebitcancel.cpp \
    TWSDNetPay_Dll/cdvjsondebitcheck.cpp \
    TWSDNetPay_Dll/cdvjsondebitsecond.cpp \
    TWSDNetPay_Dll/cdvjsonheart.cpp \
    TWSDNetPay_Dll/cdvjsonparamaccount.cpp \
    TWSDNetPay_Dll/cdvjsonparamdebitcancel.cpp \
    TWSDNetPay_Dll/cdvjsonparamdebitmoney.cpp \
    TWSDNetPay_Dll/cdvjsonparamdebitresult.cpp \
    TWSDNetPay_Dll/cdvjsonparammmi.cpp \
    TWSDNetPay_Dll/cdvxmlaccountcontext.cpp \
    TWSDNetPay_Dll/cdvxmlbase.cpp \
    TWSDNetPay_Dll/cdvxmldebitcancel.cpp \
    TWSDNetPay_Dll/cdvxmldebitparamcontext.cpp \
    TWSDNetPay_Dll/cdvxmldebitresult.cpp \
    TWSDNetPay_Dll/cdvxmlmmi.cpp \
    TWSDNetPay_Dll/chtsddevicedll.cpp \
    TWSDNetPay_Dll/cjsondebitoperate.cpp \
    TWSDNetPay_Dll/cjsonheartoperate.cpp \
    TWSDNetPay_Dll/cjsonoperationif.cpp \
    TWSDNetPay_Dll/cjsonparamoperate.cpp \
    TWSDNetPay_Dll/c_logtxt.cpp \
    TWSDNetPay_Dll/cthreadbase.cpp \
    TWSDNetPay_Dll/cthreaddevice.cpp \
    TWSDNetPay_Dll/cthreadhttp.cpp \
    TWSDNetPay_Dll/cthreadmanage.cpp \
    TWSDNetPay_Dll/cthreaduppermange.cpp \
    TWSDNetPay_Dll/ctwsdnetpay_dllapp.cpp \
    TWSDNetPay_Dll/TAesClass.cpp \
    TWSDNetPay_Dll/json/json_internalarray.inl \
    TWSDNetPay_Dll/json/json_internalmap.inl \
    TWSDNetPay_Dll/json/json_reader.cpp \
    TWSDNetPay_Dll/json/json_value.cpp \
    TWSDNetPay_Dll/json/json_valueiterator.inl \
    TWSDNetPay_Dll/json/json_writer.cpp \
    main.cpp \
    AgreementShell/cdvjsoncommanddebit.cpp \
    AgreementShell/cdvjsoncommanddebitmoney.cpp \
    AgreementShell/cdvjsoncommanddebitcencel.cpp \
    AgreementShell/commandmanagement.cpp \
    AgreementShell/cdvjsoncommanddebitinit.cpp \
    HWPerformance/hwperformance.cpp \
    HWPerformance/cxmldisplaycontext.cpp \
    AgreementShell/cdvjsoncommanddebitresult.cpp \
    AgreementShell/cdvjsoncommandwrite.cpp \
    AgreementShell/deviceinfomanagement.cpp \
    AgreementShell/cdvjsoncommandsetmmi.cpp \
    AgreementShell/miscs/twslocalnet.cpp \
    TWSDNetPay_Dll/crypt/Aes.cpp \
    TWSDNetPay_Dll/crypt/AesECB.cpp \
    TWSDNetPay_Dll/paydbinf.cpp \
    common/twsutils.cpp \
    common/twsconfig.cpp \
    upay/upaytmc.cpp \
    upay/upayutils.cpp \
    deviceManager/mgtdevice.cpp \
    deviceManager/devicemgtthread.cpp \
    PaySystem.cpp

HEADERS += \
    TWSDNetPay_Dll/basedefine.h \
    TWSDNetPay_Dll/cdvcallbackthread.h \
    TWSDNetPay_Dll/cdvjsonbase.h \
    TWSDNetPay_Dll/cdvjsondebit.h \
    TWSDNetPay_Dll/cdvjsondebitcancel.h \
    TWSDNetPay_Dll/cdvjsondebitcheck.h \
    TWSDNetPay_Dll/cdvjsondebitsecond.h \
    TWSDNetPay_Dll/cdvjsonheart.h \
    TWSDNetPay_Dll/cdvjsonparamaccount.h \
    TWSDNetPay_Dll/cdvjsonparamdebitcancel.h \
    TWSDNetPay_Dll/cdvjsonparamdebitmoney.h \
    TWSDNetPay_Dll/cdvjsonparamdebitresult.h \
    TWSDNetPay_Dll/cdvjsonparammmi.h \
    TWSDNetPay_Dll/cdvxmlaccountcontext.h \
    TWSDNetPay_Dll/cdvxmlbase.h \
    TWSDNetPay_Dll/cdvxmldebitcancel.h \
    TWSDNetPay_Dll/cdvxmldebitparamcontext.h \
    TWSDNetPay_Dll/cdvxmldebitresult.h \
    TWSDNetPay_Dll/cdvxmlmmi.h \
    TWSDNetPay_Dll/chtsddevicedll.h \
    TWSDNetPay_Dll/cjsondebitoperate.h \
    TWSDNetPay_Dll/cjsonheartoperate.h \
    TWSDNetPay_Dll/cjsonoperationif.h \
    TWSDNetPay_Dll/cjsonparamoperate.h \
    TWSDNetPay_Dll/c_logtxt.h \
    TWSDNetPay_Dll/cthreadbase.h \
    TWSDNetPay_Dll/cthreaddevice.h \
    TWSDNetPay_Dll/cthreadhttp.h \
    TWSDNetPay_Dll/cthreadmanage.h \
    TWSDNetPay_Dll/cthreaduppermange.h \
    TWSDNetPay_Dll/ctwsdnetpay_dllapp.h \
    TWSDNetPay_Dll/TAesClass.h \
    TWSDNetPay_Dll/json/autolink.h \
    TWSDNetPay_Dll/json/config.h \
    TWSDNetPay_Dll/json/features.h \
    TWSDNetPay_Dll/json/forwards.h \
    TWSDNetPay_Dll/json/json.h \
    TWSDNetPay_Dll/json/json_batchallocator.h \
    TWSDNetPay_Dll/json/reader.h \
    TWSDNetPay_Dll/json/value.h \
    TWSDNetPay_Dll/json/writer.h \
    TWSDNetPay_Dll/twsdnetpay_dll_global.h \
    AgreementShell/cdvjsoncommanddebit.h \
    AgreementShell/cdvjsoncommanddebitmoney.h \
    AgreementShell/cdvjsoncommanddebitcencel.h \
    AgreementShell/commandmanagement.h \
    AgreementShell/cdvjsoncommanddebitinit.h \
    HWPerformance/hwperformance.h \
    HWPerformance/mystdafx.h \
    HWPerformance/cxmldisplaycontext.h \
    AgreementShell/cdvjsoncommanddebitresult.h \
    AgreementShell/cdvjsoncommandwrite.h \
    AgreementShell/deviceinfomanagement.h \
    AgreementShell/cdvjsoncommandsetmmi.h \
    HWPerformance/httwsddevice.h \
    HWPerformance/httwsddevice_global.h \
    AgreementShell/miscs/twslocalnet.h \
    TWSDNetPay_Dll/crypt/Aes.h \
    TWSDNetPay_Dll/crypt/AesECB.h \
    TWSDNetPay_Dll/paydbinf.h \
    common/twsutils.h \
    common/twsconfig.h \
    upay/upaytmc.h \
    upay/upayutils.h \
    common/http/qreplytimeout.h \
    deviceManager/mgtdevice.h \
    deviceManager/devicemgtthread.h \
    PaySystem.h



unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    TWSDNetPay_Dll/json/sconscript \
    TWSDNetPay_Dll/crypt/Aes_Code.txt

INCLUDEPATH += \
    $$PWD/TWSDNetPay_Dll/                                 \
    $$PWD/HWPerformance/

#底層庫依賴
unix:!macx: LIBS += -L$$PWD/HWPerformance/lib/ -lhardware

unix:!macx: LIBS += -L$$PWD/HWPerformance/lib/ -lHTTWSDDevice



