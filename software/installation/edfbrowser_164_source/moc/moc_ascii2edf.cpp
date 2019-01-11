/****************************************************************************
** Meta object code from reading C++ file 'ascii2edf.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ascii2edf.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ascii2edf.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_ASCII2EDFapp[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      42,   16,   16,   16, 0x08,
      60,   16,   16,   16, 0x08,
      80,   16,   16,   16, 0x08,
     100,   16,   16,   16, 0x08,
     120,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_ASCII2EDFapp[] = {
    "UI_ASCII2EDFapp\0\0numofcolumnschanged(int)\0"
    "gobuttonpressed()\0savebuttonpressed()\0"
    "loadbuttonpressed()\0helpbuttonpressed()\0"
    "autoPhysicalMaximumCheckboxChanged(int)\0"
};

void UI_ASCII2EDFapp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_ASCII2EDFapp *_t = static_cast<UI_ASCII2EDFapp *>(_o);
        switch (_id) {
        case 0: _t->numofcolumnschanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->gobuttonpressed(); break;
        case 2: _t->savebuttonpressed(); break;
        case 3: _t->loadbuttonpressed(); break;
        case 4: _t->helpbuttonpressed(); break;
        case 5: _t->autoPhysicalMaximumCheckboxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_ASCII2EDFapp::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_ASCII2EDFapp::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UI_ASCII2EDFapp,
      qt_meta_data_UI_ASCII2EDFapp, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_ASCII2EDFapp::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_ASCII2EDFapp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_ASCII2EDFapp::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_ASCII2EDFapp))
        return static_cast<void*>(const_cast< UI_ASCII2EDFapp*>(this));
    return QObject::qt_metacast(_clname);
}

int UI_ASCII2EDFapp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
