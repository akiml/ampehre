/****************************************************************************
** Meta object code from reading C++ file 'QMSMsettings.h'
**
** Created: Sat Feb 18 19:03:35 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/gui/QMSMsettings.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMSMsettings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMSMSettings[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      29,   13,   13,   13, 0x05,
      43,   13,   13,   13, 0x05,
      63,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      84,   13,   13,   13, 0x0a,
      97,   13,   13,   13, 0x0a,
     111,  109,   13,   13, 0x0a,
     129,  109,   13,   13, 0x0a,
     148,  109,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QMSMSettings[] = {
    "QMSMSettings\0\0signal_start()\0signal_stop()\0"
    "signal_guiRate(int)\0signal_dataPlot(int)\0"
    "emit_start()\0emit_stop()\0v\0emit_guiRate(int)\0"
    "emit_dataPlot(int)\0"
    "setFreqLabels(std::vector<uint64_t>&)\0"
};

const QMetaObject QMSMSettings::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QMSMSettings,
      qt_meta_data_QMSMSettings, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QMSMSettings::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QMSMSettings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QMSMSettings::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMSMSettings))
        return static_cast<void*>(const_cast< QMSMSettings*>(this));
    return QWidget::qt_metacast(_clname);
}

int QMSMSettings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: signal_start(); break;
        case 1: signal_stop(); break;
        case 2: signal_guiRate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: signal_dataPlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: emit_start(); break;
        case 5: emit_stop(); break;
        case 6: emit_guiRate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: emit_dataPlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: setFreqLabels((*reinterpret_cast< std::vector<uint64_t>(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void QMSMSettings::signal_start()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QMSMSettings::signal_stop()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QMSMSettings::signal_guiRate(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QMSMSettings::signal_dataPlot(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
