/****************************************************************************
** Meta object code from reading C++ file 'QMSMsystemoverview.h'
**
** Created: Mon Apr 17 12:36:19 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/gui/QMSMsystemoverview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMSMsystemoverview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMSMSystemOverview[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   20,   19,   19, 0x0a,
      55,   51,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QMSMSystemOverview[] = {
    "QMSMSystemOverview\0\0v\0"
    "update(std::vector<double>&)\0str\0"
    "updatePID(std::vector<std::string>&)\0"
};

const QMetaObject QMSMSystemOverview::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QMSMSystemOverview,
      qt_meta_data_QMSMSystemOverview, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QMSMSystemOverview::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QMSMSystemOverview::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QMSMSystemOverview::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMSMSystemOverview))
        return static_cast<void*>(const_cast< QMSMSystemOverview*>(this));
    return QWidget::qt_metacast(_clname);
}

int QMSMSystemOverview::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: update((*reinterpret_cast< std::vector<double>(*)>(_a[1]))); break;
        case 1: updatePID((*reinterpret_cast< std::vector<std::string>(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
