/****************************************************************************
** Meta object code from reading C++ file 'import_annotations.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../import_annotations.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'import_annotations.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_ImportAnnotationswindow[] = {

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
      28,   27,   27,   27, 0x08,
      50,   27,   27,   27, 0x08,
      86,   27,   27,   27, 0x08,
     112,   27,   27,   27, 0x08,
     141,   27,   27,   27, 0x08,
     157,   27,   27,   27, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_ImportAnnotationswindow[] = {
    "UI_ImportAnnotationswindow\0\0"
    "ImportButtonClicked()\0"
    "descriptionRadioButtonClicked(bool)\0"
    "DCEventSignalChanged(int)\0"
    "DurationCheckBoxChanged(int)\0"
    "TabChanged(int)\0helpbuttonpressed()\0"
};

void UI_ImportAnnotationswindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_ImportAnnotationswindow *_t = static_cast<UI_ImportAnnotationswindow *>(_o);
        switch (_id) {
        case 0: _t->ImportButtonClicked(); break;
        case 1: _t->descriptionRadioButtonClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->DCEventSignalChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->DurationCheckBoxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->TabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->helpbuttonpressed(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_ImportAnnotationswindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_ImportAnnotationswindow::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UI_ImportAnnotationswindow,
      qt_meta_data_UI_ImportAnnotationswindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_ImportAnnotationswindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_ImportAnnotationswindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_ImportAnnotationswindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_ImportAnnotationswindow))
        return static_cast<void*>(const_cast< UI_ImportAnnotationswindow*>(this));
    return QObject::qt_metacast(_clname);
}

int UI_ImportAnnotationswindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
