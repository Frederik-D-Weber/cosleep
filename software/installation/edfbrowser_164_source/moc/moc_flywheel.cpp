/****************************************************************************
** Meta object code from reading C++ file 'flywheel.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../flywheel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'flywheel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_Flywheel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      28,   12,   12,   12, 0x08,
      43,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_Flywheel[] = {
    "UI_Flywheel\0\0dialMoved(int)\0wheel_rotate()\0"
    "t2_time_out()\0"
};

void UI_Flywheel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_Flywheel *_t = static_cast<UI_Flywheel *>(_o);
        switch (_id) {
        case 0: _t->dialMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->wheel_rotate(); break;
        case 2: _t->t2_time_out(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_Flywheel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_Flywheel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_UI_Flywheel,
      qt_meta_data_UI_Flywheel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_Flywheel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_Flywheel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_Flywheel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_Flywheel))
        return static_cast<void*>(const_cast< UI_Flywheel*>(this));
    return QWidget::qt_metacast(_clname);
}

int UI_Flywheel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void UI_Flywheel::dialMoved(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
