/****************************************************************************
** Meta object code from reading C++ file 'special_button.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../special_button.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'special_button.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpecialButton[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      39,   14,   14,   14, 0x0a,
      63,   14,   56,   14, 0x0a,
      71,   14,   14,   14, 0x0a,
      95,   14,   91,   14, 0x0a,
     109,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SpecialButton[] = {
    "SpecialButton\0\0clicked(SpecialButton*)\0"
    "setColor(QColor)\0QColor\0color()\0"
    "setGlobalColor(int)\0int\0globalColor()\0"
    "setText(const char*)\0"
};

void SpecialButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpecialButton *_t = static_cast<SpecialButton *>(_o);
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast< SpecialButton*(*)>(_a[1]))); break;
        case 1: _t->setColor((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 2: { QColor _r = _t->color();
            if (_a[0]) *reinterpret_cast< QColor*>(_a[0]) = _r; }  break;
        case 3: _t->setGlobalColor((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: { int _r = _t->globalColor();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: _t->setText((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SpecialButton::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpecialButton::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SpecialButton,
      qt_meta_data_SpecialButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpecialButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpecialButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpecialButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpecialButton))
        return static_cast<void*>(const_cast< SpecialButton*>(this));
    return QWidget::qt_metacast(_clname);
}

int SpecialButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SpecialButton::clicked(SpecialButton * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
