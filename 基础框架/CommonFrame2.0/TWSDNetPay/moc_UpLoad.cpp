/****************************************************************************
** Meta object code from reading C++ file 'UpLoad.h'
**
** Created: Thu Nov 23 09:59:05 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "UpLoad.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UpLoad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UpLoad[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,    8,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      55,   49,    7,    7, 0x08,
      95,   85,    7,    7, 0x08,
     157,  136,    7,    7, 0x08,
     189,    7,    7,    7, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UpLoad[] = {
    "UpLoad\0\0retCode,msg\0sigReplyMessage(int,QString)\0"
    "reply\0replyFinished(QNetworkReply*)\0"
    "errorCode\0upLoadError(QNetworkReply::NetworkError)\0"
    "bytesSent,bytesTotal\0"
    "OnUploadProgress(qint64,qint64)\0"
    "slotUploadFinished()\0"
};

const QMetaObject UpLoad::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UpLoad,
      qt_meta_data_UpLoad, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UpLoad::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UpLoad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UpLoad::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UpLoad))
        return static_cast<void*>(const_cast< UpLoad*>(this));
    return QObject::qt_metacast(_clname);
}

int UpLoad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sigReplyMessage((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: replyFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 2: upLoadError((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 3: OnUploadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 4: slotUploadFinished(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void UpLoad::sigReplyMessage(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
