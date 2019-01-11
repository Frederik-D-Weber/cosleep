/****************************************************************************
** Meta object code from reading C++ file 'nexfin2edf.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../nexfin2edf.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nexfin2edf.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_NEXFIN2EDFwindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x08,
      40,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_NEXFIN2EDFwindow[] = {
    "UI_NEXFIN2EDFwindow\0\0SelectFileButton()\0"
    "radiobuttonchecked(bool)\0"
};

void UI_NEXFIN2EDFwindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_NEXFIN2EDFwindow *_t = static_cast<UI_NEXFIN2EDFwindow *>(_o);
        switch (_id) {
        case 0: _t->SelectFileButton(); break;
        case 1: _t->radiobuttonchecked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_NEXFIN2EDFwindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_NEXFIN2EDFwindow::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UI_NEXFIN2EDFwindow,
      qt_meta_data_UI_NEXFIN2EDFwindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_NEXFIN2EDFwindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_NEXFIN2EDFwindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_NEXFIN2EDFwindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_NEXFIN2EDFwindow))
        return static_cast<void*>(const_cast< UI_NEXFIN2EDFwindow*>(this));
    return QObject::qt_metacast(_clname);
}

int UI_NEXFIN2EDFwindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
