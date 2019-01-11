/****************************************************************************
** Meta object code from reading C++ file 'spectrumanalyzer.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../spectrumanalyzer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spectrumanalyzer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_FreqSpectrumWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x08,
      38,   22,   22,   22, 0x08,
      55,   22,   22,   22, 0x08,
      89,   22,   22,   22, 0x08,
     104,   22,   22,   22, 0x08,
     125,   22,   22,   22, 0x08,
     145,   22,   22,   22, 0x08,
     170,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_FreqSpectrumWindow[] = {
    "UI_FreqSpectrumWindow\0\0update_curve()\0"
    "sliderMoved(int)\0SpectrumDialogDestroyed(QObject*)\0"
    "print_to_txt()\0update_flywheel(int)\0"
    "thr_finished_func()\0dftsz_value_changed(int)\0"
    "windowBox_changed(int)\0"
};

void UI_FreqSpectrumWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_FreqSpectrumWindow *_t = static_cast<UI_FreqSpectrumWindow *>(_o);
        switch (_id) {
        case 0: _t->update_curve(); break;
        case 1: _t->sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->SpectrumDialogDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 3: _t->print_to_txt(); break;
        case 4: _t->update_flywheel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->thr_finished_func(); break;
        case 6: _t->dftsz_value_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->windowBox_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_FreqSpectrumWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_FreqSpectrumWindow::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_UI_FreqSpectrumWindow,
      qt_meta_data_UI_FreqSpectrumWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_FreqSpectrumWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_FreqSpectrumWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_FreqSpectrumWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_FreqSpectrumWindow))
        return static_cast<void*>(const_cast< UI_FreqSpectrumWindow*>(this));
    return QThread::qt_metacast(_clname);
}

int UI_FreqSpectrumWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
