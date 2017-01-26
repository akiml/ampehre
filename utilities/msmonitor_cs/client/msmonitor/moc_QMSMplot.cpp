/****************************************************************************
** Meta object code from reading C++ file 'QMSMplot.h'
**
** Created: Thu Jan 26 15:49:29 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/gui/QMSMplot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMSMplot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMSMplot[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   10,    9,    9, 0x0a,
      52,    9,    9,    9, 0x0a,
      68,   61,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QMSMplot[] = {
    "QMSMplot\0\0values\0updateValues(std::vector<double>&)\0"
    "redraw()\0parent\0initPlot(QWidget*)\0"
};

const QMetaObject QMSMplot::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QMSMplot,
      qt_meta_data_QMSMplot, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QMSMplot::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QMSMplot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QMSMplot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMSMplot))
        return static_cast<void*>(const_cast< QMSMplot*>(this));
    return QWidget::qt_metacast(_clname);
}

int QMSMplot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateValues((*reinterpret_cast< std::vector<double>(*)>(_a[1]))); break;
        case 1: redraw(); break;
        case 2: initPlot((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
