/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Mon Jan 30 10:45:59 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/gui/mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
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
      90,   11,   11,   11, 0x0a,
     102,   11,   11,   11, 0x0a,
     113,   11,   11,   11, 0x0a,
     125,   11,   11,   11, 0x0a,
     136,   11,   11,   11, 0x0a,
     149,   11,   11,   11, 0x0a,
     167,   11,   11,   11, 0x0a,
     185,   11,   11,   11, 0x0a,
     199,   11,   11,   11, 0x0a,
     212,   11,   11,   11, 0x0a,
     226,   11,   11,   11, 0x0a,
     239,   11,   11,   11, 0x0a,
     254,   11,   11,   11, 0x0a,
     274,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0start()\0stop()\0reset()\0"
    "requestData()\0val\0setGuiInterval(int)\0"
    "setInterval(int)\0showPower()\0showTemp()\0"
    "showClock()\0showUtil()\0showMemory()\0"
    "showHeatmapUtil()\0showHeatmapTemp()\0"
    "updatePower()\0updateTemp()\0updateClock()\0"
    "updateUtil()\0updateMemory()\0"
    "updateHeatmapUtil()\0updateHeatmapTemp()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
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
        switch (_id) {
        case 0: start(); break;
        case 1: stop(); break;
        case 2: reset(); break;
        case 3: requestData(); break;
        case 4: setGuiInterval((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: setInterval((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: showPower(); break;
        case 7: showTemp(); break;
        case 8: showClock(); break;
        case 9: showUtil(); break;
        case 10: showMemory(); break;
        case 11: showHeatmapUtil(); break;
        case 12: showHeatmapTemp(); break;
        case 13: updatePower(); break;
        case 14: updateTemp(); break;
        case 15: updateClock(); break;
        case 16: updateUtil(); break;
        case 17: updateMemory(); break;
        case 18: updateHeatmapUtil(); break;
        case 19: updateHeatmapTemp(); break;
        default: ;
        }
        _id -= 20;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
