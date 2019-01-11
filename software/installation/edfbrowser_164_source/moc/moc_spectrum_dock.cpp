/****************************************************************************
** Meta object code from reading C++ file 'spectrum_dock.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../spectrum_dock.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spectrum_dock.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_SpectrumDockWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x08,
      38,   22,   22,   22, 0x08,
      55,   22,   22,   22, 0x08,
      79,   22,   22,   22, 0x08,
     103,   22,   22,   22, 0x08,
     131,   22,   22,   22, 0x08,
     146,   22,   22,   22, 0x08,
     161,   22,   22,   22, 0x08,
     182,   22,   22,   22, 0x08,
     207,   22,   22,   22, 0x08,
     230,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_SpectrumDockWindow[] = {
    "UI_SpectrumDockWindow\0\0update_curve()\0"
    "sliderMoved(int)\0sqrtButtonClicked(bool)\0"
    "vlogButtonClicked(bool)\0"
    "colorBarButtonClicked(bool)\0print_to_txt()\0"
    "setdashboard()\0update_flywheel(int)\0"
    "dftsz_value_changed(int)\0"
    "windowBox_changed(int)\0open_close_dock(bool)\0"
};

void UI_SpectrumDockWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_SpectrumDockWindow *_t = static_cast<UI_SpectrumDockWindow *>(_o);
        switch (_id) {
        case 0: _t->update_curve(); break;
        case 1: _t->sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->sqrtButtonClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->vlogButtonClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->colorBarButtonClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->print_to_txt(); break;
        case 6: _t->setdashboard(); break;
        case 7: _t->update_flywheel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->dftsz_value_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->windowBox_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->open_close_dock((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_SpectrumDockWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_SpectrumDockWindow::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UI_SpectrumDockWindow,
      qt_meta_data_UI_SpectrumDockWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_SpectrumDockWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_SpectrumDockWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_SpectrumDockWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_SpectrumDockWindow))
        return static_cast<void*>(const_cast< UI_SpectrumDockWindow*>(this));
    return QObject::qt_metacast(_clname);
}

int UI_SpectrumDockWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
