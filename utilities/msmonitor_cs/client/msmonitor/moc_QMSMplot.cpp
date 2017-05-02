/****************************************************************************
** Meta object code from reading C++ file 'QMSMplot.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/gui/QMSMplot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMSMplot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMSMplot[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      42,   35,    9,    9, 0x0a,
      77,    9,    9,    9, 0x0a,
      86,    9,    9,    9, 0x0a,
      97,    9,    9,    9, 0x0a,
     115,  108,    9,    9, 0x0a,
     140,  133,    9,    9, 0x0a,
     160,    9,    9,    9, 0x0a,
     171,    9,    9,    9, 0x0a,
     184,    9,    9,    9, 0x0a,
     201,  199,    9,    9, 0x0a,
     217,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QMSMplot[] = {
    "QMSMplot\0\0signal_export(QMSMplot*)\0"
    "values\0updateValues(std::vector<double>&)\0"
    "redraw()\0initPlot()\0makeGrid()\0xValue\0"
    "scaleAxis(double)\0lValue\0resetLineWidth(int)\0"
    "resetPen()\0screenshot()\0makeZoomable()\0"
    "v\0setMaxData(int)\0exportToCSV()\0"
};

void QMSMplot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QMSMplot *_t = static_cast<QMSMplot *>(_o);
        switch (_id) {
        case 0: _t->signal_export((*reinterpret_cast< QMSMplot*(*)>(_a[1]))); break;
        case 1: _t->updateValues((*reinterpret_cast< std::vector<double>(*)>(_a[1]))); break;
        case 2: _t->redraw(); break;
        case 3: _t->initPlot(); break;
        case 4: _t->makeGrid(); break;
        case 5: _t->scaleAxis((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->resetLineWidth((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->resetPen(); break;
        case 8: _t->screenshot(); break;
        case 9: _t->makeZoomable(); break;
        case 10: _t->setMaxData((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->exportToCSV(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QMSMplot::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QMSMplot::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QMSMplot,
      qt_meta_data_QMSMplot, &staticMetaObjectExtraData }
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
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void QMSMplot::signal_export(QMSMplot * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
