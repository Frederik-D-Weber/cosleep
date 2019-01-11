/****************************************************************************
** Meta object code from reading C++ file 'signal_chooser.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../signal_chooser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'signal_chooser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_SignalChooser[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x08,
      50,   17,   17,   17, 0x08,
      61,   17,   17,   17, 0x08,
      74,   17,   17,   17, 0x08,
      89,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_SignalChooser[] = {
    "UI_SignalChooser\0\0call_sidemenu(QListWidgetItem*)\0"
    "signalUp()\0signalDown()\0signalDelete()\0"
    "signalInvert()\0"
};

void UI_SignalChooser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_SignalChooser *_t = static_cast<UI_SignalChooser *>(_o);
        switch (_id) {
        case 0: _t->call_sidemenu((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 1: _t->signalUp(); break;
        case 2: _t->signalDown(); break;
        case 3: _t->signalDelete(); break;
        case 4: _t->signalInvert(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_SignalChooser::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_SignalChooser::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UI_SignalChooser,
      qt_meta_data_UI_SignalChooser, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_SignalChooser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_SignalChooser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_SignalChooser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_SignalChooser))
        return static_cast<void*>(const_cast< UI_SignalChooser*>(this));
    return QObject::qt_metacast(_clname);
}

int UI_SignalChooser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
