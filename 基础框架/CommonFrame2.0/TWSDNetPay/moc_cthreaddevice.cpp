/****************************************************************************
** Meta object code from reading C++ file 'cthreaddevice.h'
**
** Created: Thu Nov 23 09:58:58 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "cthreaddevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cthreaddevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CThreadDevice[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x05,
      49,   15,   14,   14, 0x05,
      96,   15,   14,   14, 0x05,
     139,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     198,  184,   14,   14, 0x0a,
     229,  184,   14,   14, 0x0a,
     262,  184,   14,   14, 0x0a,
     296,   14,   14,   14, 0x0a,
     322,   14,   14,   14, 0x0a,
     344,   14,   14,   14, 0x0a,
     381,  364,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CThreadDevice[] = {
    "CThreadDevice\0\0,\0SENDCALLBACKRESULT(int,QString)\0"
    "WM_THREAD_DEVICE_START_DEBIT_FLOW(QString,int)\0"
    "WM_THREAD_DEVICE_DEBIT_CANCEL(QString,int)\0"
    "WM_THREAD_DEVICE_SHOW_DEAL_INFO(QString,int)\0"
    "wParam,lParam\0OnMsgShowDealInfo(QString,int)\0"
    "OnMsgStartDebitFlow(QString,int)\0"
    "OnMsgDebitCancelFlow(QString,int)\0"
    "OnTimerInitConfigDevice()\0"
    "OnTimerActiveThread()\0OnTimerInitDevice()\0"
    "nType,dataResult\0OnMsgCallbackResult(int,QString)\0"
};

const QMetaObject CThreadDevice::staticMetaObject = {
    { &CThreadBase::staticMetaObject, qt_meta_stringdata_CThreadDevice,
      qt_meta_data_CThreadDevice, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CThreadDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CThreadDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CThreadDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CThreadDevice))
        return static_cast<void*>(const_cast< CThreadDevice*>(this));
    return CThreadBase::qt_metacast(_clname);
}

int CThreadDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CThreadBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: SENDCALLBACKRESULT((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: WM_THREAD_DEVICE_START_DEBIT_FLOW((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: WM_THREAD_DEVICE_DEBIT_CANCEL((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: WM_THREAD_DEVICE_SHOW_DEAL_INFO((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: OnMsgShowDealInfo((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: OnMsgStartDebitFlow((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: OnMsgDebitCancelFlow((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: OnTimerInitConfigDevice(); break;
        case 8: OnTimerActiveThread(); break;
        case 9: OnTimerInitDevice(); break;
        case 10: OnMsgCallbackResult((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void CThreadDevice::SENDCALLBACKRESULT(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CThreadDevice::WM_THREAD_DEVICE_START_DEBIT_FLOW(QString _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CThreadDevice::WM_THREAD_DEVICE_DEBIT_CANCEL(QString _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CThreadDevice::WM_THREAD_DEVICE_SHOW_DEAL_INFO(QString _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
