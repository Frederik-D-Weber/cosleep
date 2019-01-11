/****************************************************************************
** Meta object code from reading C++ file 'colordialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../colordialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'colordialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_ColorMenuDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      57,   19,   19,   19, 0x08,
      94,   19,   19,   19, 0x08,
     131,   19,   19,   19, 0x08,
     168,   19,   19,   19, 0x08,
     205,   19,   19,   19, 0x08,
     242,   19,   19,   19, 0x08,
     279,   19,   19,   19, 0x08,
     316,   19,   19,   19, 0x08,
     353,   19,   19,   19, 0x08,
     391,   19,   19,   19, 0x08,
     429,   19,   19,   19, 0x08,
     467,   19,   19,   19, 0x08,
     505,   19,   19,   19, 0x08,
     543,   19,   19,   19, 0x08,
     581,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_ColorMenuDialog[] = {
    "UI_ColorMenuDialog\0\0"
    "ColorButton1_pressed(SpecialButton*)\0"
    "ColorButton2_pressed(SpecialButton*)\0"
    "ColorButton3_pressed(SpecialButton*)\0"
    "ColorButton4_pressed(SpecialButton*)\0"
    "ColorButton5_pressed(SpecialButton*)\0"
    "ColorButton6_pressed(SpecialButton*)\0"
    "ColorButton7_pressed(SpecialButton*)\0"
    "ColorButton8_pressed(SpecialButton*)\0"
    "ColorButton9_pressed(SpecialButton*)\0"
    "ColorButton10_pressed(SpecialButton*)\0"
    "ColorButton11_pressed(SpecialButton*)\0"
    "ColorButton12_pressed(SpecialButton*)\0"
    "ColorButton13_pressed(SpecialButton*)\0"
    "ColorButton14_pressed(SpecialButton*)\0"
    "ColorButton15_pressed(SpecialButton*)\0"
    "ColorButton16_pressed(SpecialButton*)\0"
};

void UI_ColorMenuDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_ColorMenuDialog *_t = static_cast<UI_ColorMenuDialog *>(_o);
        switch (_id) {
        case 0: _t->ColorButton1_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 1: _t->ColorButton2_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 2: _t->ColorButton3_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 3: _t->ColorButton4_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 4: _t->ColorButton5_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 5: _t->ColorButton6_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 6: _t->ColorButton7_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 7: _t->ColorButton8_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 8: _t->ColorButton9_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 9: _t->ColorButton10_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 10: _t->ColorButton11_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 11: _t->ColorButton12_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 12: _t->ColorButton13_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 13: _t->ColorButton14_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 14: _t->ColorButton15_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 15: _t->ColorButton16_pressed((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_ColorMenuDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_ColorMenuDialog::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UI_ColorMenuDialog,
      qt_meta_data_UI_ColorMenuDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_ColorMenuDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_ColorMenuDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_ColorMenuDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_ColorMenuDialog))
        return static_cast<void*>(const_cast< UI_ColorMenuDialog*>(this));
    return QObject::qt_metacast(_clname);
}

int UI_ColorMenuDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
