/****************************************************************************
** Meta object code from reading C++ file 'CConfig.h'
**
** Created: Mon Apr 17 12:36:18 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/CConfig.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CConfig.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CConfig[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,    9,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CConfig[] = {
    "CConfig\0\0plot\0exportPlotToCSV(QMSMplot*)\0"
};

const QMetaObject CConfig::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CConfig,
      qt_meta_data_CConfig, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CConfig::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CConfig::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CConfig::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CConfig))
        return static_cast<void*>(const_cast< CConfig*>(this));
    return QWidget::qt_metacast(_clname);
}

int CConfig::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exportPlotToCSV((*reinterpret_cast< QMSMplot*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
