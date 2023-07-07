/****************************************************************************
** Meta object code from reading C++ file 'UpdateQuery.h'
**
** Created: Wed Nov 22 18:19:51 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "UpdateQuery/UpdateQuery.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UpdateQuery.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UpdateQuery[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_UpdateQuery[] = {
    "UpdateQuery\0\0updateQuery2Http()\0"
};

const QMetaObject UpdateQuery::staticMetaObject = {
    { &UpdateCommon::staticMetaObject, qt_meta_stringdata_UpdateQuery,
      qt_meta_data_UpdateQuery, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UpdateQuery::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UpdateQuery::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UpdateQuery::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UpdateQuery))
        return static_cast<void*>(const_cast< UpdateQuery*>(this));
    if (!strcmp(_clname, "CommonBus"))
        return static_cast< CommonBus*>(const_cast< UpdateQuery*>(this));
    return UpdateCommon::qt_metacast(_clname);
}

int UpdateQuery::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = UpdateCommon::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateQuery2Http(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
