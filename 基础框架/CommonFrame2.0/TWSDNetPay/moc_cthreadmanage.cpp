/****************************************************************************
** Meta object code from reading C++ file 'cthreadmanage.h'
**
** Created: Thu Nov 23 09:58:56 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "cthreadmanage.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cthreadmanage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CThreadManage[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x05,
      62,   15,   14,   14, 0x05,
     109,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     166,  152,   14,   14, 0x0a,
     205,  152,   14,   14, 0x0a,
     235,  152,   14,   14, 0x0a,
     262,   14,   14,   14, 0x0a,
     289,   14,   14,   14, 0x0a,
     313,   14,   14,   14, 0x0a,
     338,   14,   14,   14, 0x0a,
     364,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CThreadManage[] = {
    "CThreadManage\0\0,\0"
    "WM_THREAD_MANAGE_SHOW_DEAL_INFO(QString,int)\0"
    "WM_THREAD_MANAGE_START_DEBIT_FLOW(QString,int)\0"
    "WM_THREAD_MANAGE_DEBIT_CANCEL(QString,int)\0"
    "wParam,lParam\0OnMsgManageStartDebitFlow(QString,int)\0"
    "OnMsgDebitCancel(QString,int)\0"
    "OnMsgShowInfo(QString,int)\0"
    "TimerIDForDebitMoneyProc()\0"
    "TimerIDforDeviceState()\0"
    "TimerIDForThreadActive()\0"
    "TimerIDForDeleteLogFile()\0"
    "CheckAndDeleteLogFile()\0"
};

const QMetaObject CThreadManage::staticMetaObject = {
    { &CThreadBase::staticMetaObject, qt_meta_stringdata_CThreadManage,
      qt_meta_data_CThreadManage, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CThreadManage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CThreadManage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CThreadManage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CThreadManage))
        return static_cast<void*>(const_cast< CThreadManage*>(this));
    return CThreadBase::qt_metacast(_clname);
}

int CThreadManage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CThreadBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: WM_THREAD_MANAGE_SHOW_DEAL_INFO((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: WM_THREAD_MANAGE_START_DEBIT_FLOW((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: WM_THREAD_MANAGE_DEBIT_CANCEL((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: OnMsgManageStartDebitFlow((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: OnMsgDebitCancel((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: OnMsgShowInfo((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: TimerIDForDebitMoneyProc(); break;
        case 7: TimerIDforDeviceState(); break;
        case 8: TimerIDForThreadActive(); break;
        case 9: TimerIDForDeleteLogFile(); break;
        case 10: CheckAndDeleteLogFile(); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void CThreadManage::WM_THREAD_MANAGE_SHOW_DEAL_INFO(QString _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CThreadManage::WM_THREAD_MANAGE_START_DEBIT_FLOW(QString _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CThreadManage::WM_THREAD_MANAGE_DEBIT_CANCEL(QString _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
