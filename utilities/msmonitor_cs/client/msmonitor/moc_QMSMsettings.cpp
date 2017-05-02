/****************************************************************************
** Meta object code from reading C++ file 'QMSMsettings.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/gui/QMSMsettings.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMSMsettings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMSMSettings[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      29,   13,   13,   13, 0x05,
      43,   13,   13,   13, 0x05,
      63,   13,   13,   13, 0x05,
      84,   13,   13,   13, 0x05,
     105,   13,   13,   13, 0x05,
     129,   13,   13,   13, 0x05,
     155,   13,   13,   13, 0x05,
     175,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     197,   13,   13,   13, 0x0a,
     210,   13,   13,   13, 0x0a,
     222,   13,   13,   13, 0x0a,
     229,   13,   13,   13, 0x0a,
     239,  237,   13,   13, 0x0a,
     257,  237,   13,   13, 0x0a,
     276,  237,   13,   13, 0x0a,
     297,  295,   13,   13, 0x0a,
     321,  237,   13,   13, 0x0a,
     343,   13,   13,   13, 0x0a,
     361,   13,   13,   13, 0x0a,
     381,  237,   13,   13, 0x0a,
     395,  237,   13,   13, 0x0a,
     415,   13,  411,   13, 0x0a,
     434,   13,  426,   13, 0x0a,
     443,   13,  411,   13, 0x0a,
     457,  237,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QMSMSettings[] = {
    "QMSMSettings\0\0signal_start()\0signal_stop()\0"
    "signal_guiRate(int)\0signal_dataPlot(int)\0"
    "signal_saveData(int)\0signal_portChanged(int)\0"
    "signal_ipChanged(QString)\0signal_loadConfig()\0"
    "signal_exportConfig()\0emit_start()\0"
    "emit_stop()\0stop()\0start()\0v\0"
    "emit_guiRate(int)\0emit_dataPlot(int)\0"
    "emit_saveData(int)\0s\0emit_ipChanged(QString)\0"
    "emit_portChanged(int)\0emit_loadConfig()\0"
    "emit_exportConfig()\0set_port(int)\0"
    "set_ip(QString)\0int\0get_port()\0QString\0"
    "get_ip()\0get_maxData()\0"
    "setFreqLabels(std::vector<uint64_t>&)\0"
};

void QMSMSettings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QMSMSettings *_t = static_cast<QMSMSettings *>(_o);
        switch (_id) {
        case 0: _t->signal_start(); break;
        case 1: _t->signal_stop(); break;
        case 2: _t->signal_guiRate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->signal_dataPlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->signal_saveData((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->signal_portChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->signal_ipChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->signal_loadConfig(); break;
        case 8: _t->signal_exportConfig(); break;
        case 9: _t->emit_start(); break;
        case 10: _t->emit_stop(); break;
        case 11: _t->stop(); break;
        case 12: _t->start(); break;
        case 13: _t->emit_guiRate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->emit_dataPlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->emit_saveData((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->emit_ipChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 17: _t->emit_portChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->emit_loadConfig(); break;
        case 19: _t->emit_exportConfig(); break;
        case 20: _t->set_port((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->set_ip((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 22: { int _r = _t->get_port();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 23: { QString _r = _t->get_ip();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 24: { int _r = _t->get_maxData();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 25: _t->setFreqLabels((*reinterpret_cast< std::vector<uint64_t>(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QMSMSettings::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QMSMSettings::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QMSMSettings,
      qt_meta_data_QMSMSettings, &staticMetaObjectExtraData }
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
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
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

// SIGNAL 4
void QMSMSettings::signal_saveData(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QMSMSettings::signal_portChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QMSMSettings::signal_ipChanged(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QMSMSettings::signal_loadConfig()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void QMSMSettings::signal_exportConfig()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}
QT_END_MOC_NAMESPACE
