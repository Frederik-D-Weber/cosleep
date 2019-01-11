/****************************************************************************
** Meta object code from reading C++ file 'signals_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../signals_dialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'signals_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_Signalswindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x08,
      36,   17,   17,   17, 0x08,
      61,   17,   17,   17, 0x08,
      84,   17,   17,   17, 0x08,
     111,   17,   17,   17, 0x08,
     130,   17,   17,   17, 0x08,
     154,   17,   17,   17, 0x08,
     176,   17,   17,   17, 0x08,
     211,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_Signalswindow[] = {
    "UI_Signalswindow\0\0show_signals(int)\0"
    "SelectAllButtonClicked()\0"
    "DisplayButtonClicked()\0"
    "DisplayCompButtonClicked()\0"
    "AddButtonClicked()\0SubtractButtonClicked()\0"
    "RemoveButtonClicked()\0"
    "ColorButtonClicked(SpecialButton*)\0"
    "HelpButtonClicked()\0"
};

void UI_Signalswindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_Signalswindow *_t = static_cast<UI_Signalswindow *>(_o);
        switch (_id) {
        case 0: _t->show_signals((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->SelectAllButtonClicked(); break;
        case 2: _t->DisplayButtonClicked(); break;
        case 3: _t->DisplayCompButtonClicked(); break;
        case 4: _t->AddButtonClicked(); break;
        case 5: _t->SubtractButtonClicked(); break;
        case 6: _t->RemoveButtonClicked(); break;
        case 7: _t->ColorButtonClicked((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 8: _t->HelpButtonClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_Signalswindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_Signalswindow::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UI_Signalswindow,
      qt_meta_data_UI_Signalswindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_Signalswindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_Signalswindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_Signalswindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_Signalswindow))
        return static_cast<void*>(const_cast< UI_Signalswindow*>(this));
    return QObject::qt_metacast(_clname);
}

int UI_Signalswindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
