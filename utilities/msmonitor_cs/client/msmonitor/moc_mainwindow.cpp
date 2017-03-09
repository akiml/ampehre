/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Thu Mar 9 19:18:28 2017
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
      22,   14, // methods
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
     230,   11,   11,   11, 0x0a,
     243,   11,   11,   11, 0x0a,
     257,   11,   11,   11, 0x0a,
     270,   11,   11,   11, 0x0a,
     285,   11,   11,   11, 0x0a,
     305,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0start()\0stop()\0reset()\0"
    "requestData()\0val\0setGuiInterval(int)\0"
    "setInterval(int)\0setMaxData(int)\0"
    "showPower()\0showTemp()\0showClock()\0"
    "showUtil()\0showMemory()\0showHeatmapUtil()\0"
    "showHeatmapTemp()\0showSettings()\0"
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
        case 6: setMaxData((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: showPower(); break;
        case 8: showTemp(); break;
        case 9: showClock(); break;
        case 10: showUtil(); break;
        case 11: showMemory(); break;
        case 12: showHeatmapUtil(); break;
        case 13: showHeatmapTemp(); break;
        case 14: showSettings(); break;
        case 15: updatePower(); break;
        case 16: updateTemp(); break;
        case 17: updateClock(); break;
        case 18: updateUtil(); break;
        case 19: updateMemory(); break;
        case 20: updateHeatmapUtil(); break;
        case 21: updateHeatmapTemp(); break;
        default: ;
        }
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
