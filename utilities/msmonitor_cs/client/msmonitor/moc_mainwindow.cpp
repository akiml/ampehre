/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/gui/mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      20,   11,   11,   11, 0x0a,
      27,   11,   11,   11, 0x0a,
      35,   11,   11,   11, 0x0a,
      53,   49,   11,   11, 0x0a,
      73,   49,   11,   11, 0x0a,
      90,   49,   11,   11, 0x0a,
     106,   11,   11,   11, 0x0a,
     118,   11,   11,   11, 0x0a,
     129,   11,   11,   11, 0x0a,
     141,   11,   11,   11, 0x0a,
     152,   11,   11,   11, 0x0a,
     165,   11,   11,   11, 0x0a,
     183,   11,   11,   11, 0x0a,
     201,   11,   11,   11, 0x0a,
     216,   11,   11,   11, 0x0a,
     237,   11,   11,   11, 0x0a,
     255,   11,   11,   11, 0x0a,
     269,   11,   11,   11, 0x0a,
     282,   11,   11,   11, 0x0a,
     296,   11,   11,   11, 0x0a,
     309,   11,   11,   11, 0x0a,
     324,   11,   11,   11, 0x0a,
     344,   11,   11,   11, 0x0a,
     364,   11,   11,   11, 0x0a,
     387,   11,   11,   11, 0x0a,
     402,   11,   11,   11, 0x0a,
     419,  417,   11,   11, 0x0a,
     440,  417,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0start()\0stop()\0reset()\0"
    "requestData()\0val\0setGuiInterval(int)\0"
    "setInterval(int)\0setMaxData(int)\0"
    "showPower()\0showTemp()\0showClock()\0"
    "showUtil()\0showMemory()\0showHeatmapUtil()\0"
    "showHeatmapTemp()\0showSettings()\0"
    "showSystemOverview()\0setInitSettings()\0"
    "updatePower()\0updateTemp()\0updateClock()\0"
    "updateUtil()\0updateMemory()\0"
    "updateHeatmapUtil()\0updateHeatmapTemp()\0"
    "updateSystemOverview()\0exportConfig()\0"
    "importConfig()\0v\0setServerIP(QString)\0"
    "setServerPort(int)\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->start(); break;
        case 1: _t->stop(); break;
        case 2: _t->reset(); break;
        case 3: _t->requestData(); break;
        case 4: _t->setGuiInterval((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->setInterval((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->setMaxData((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->showPower(); break;
        case 8: _t->showTemp(); break;
        case 9: _t->showClock(); break;
        case 10: _t->showUtil(); break;
        case 11: _t->showMemory(); break;
        case 12: _t->showHeatmapUtil(); break;
        case 13: _t->showHeatmapTemp(); break;
        case 14: _t->showSettings(); break;
        case 15: _t->showSystemOverview(); break;
        case 16: _t->setInitSettings(); break;
        case 17: _t->updatePower(); break;
        case 18: _t->updateTemp(); break;
        case 19: _t->updateClock(); break;
        case 20: _t->updateUtil(); break;
        case 21: _t->updateMemory(); break;
        case 22: _t->updateHeatmapUtil(); break;
        case 23: _t->updateHeatmapTemp(); break;
        case 24: _t->updateSystemOverview(); break;
        case 25: _t->exportConfig(); break;
        case 26: _t->importConfig(); break;
        case 27: _t->setServerIP((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 28: _t->setServerPort((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
