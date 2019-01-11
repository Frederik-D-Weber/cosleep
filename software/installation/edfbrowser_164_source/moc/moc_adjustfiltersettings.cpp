/****************************************************************************
** Meta object code from reading C++ file 'adjustfiltersettings.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../adjustfiltersettings.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'adjustfiltersettings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AdjustFilterSettings[] = {

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
      22,   21,   21,   21, 0x08,
      51,   21,   21,   21, 0x08,
      80,   21,   21,   21, 0x08,
     106,   21,   21,   21, 0x08,
     130,   21,   21,   21, 0x08,
     152,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_AdjustFilterSettings[] = {
    "AdjustFilterSettings\0\0"
    "freqbox1valuechanged(double)\0"
    "freqbox2valuechanged(double)\0"
    "orderboxvaluechanged(int)\0"
    "stepsizeboxchanged(int)\0filterboxchanged(int)\0"
    "removeButtonClicked()\0"
};

void AdjustFilterSettings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AdjustFilterSettings *_t = static_cast<AdjustFilterSettings *>(_o);
        switch (_id) {
        case 0: _t->freqbox1valuechanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->freqbox2valuechanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->orderboxvaluechanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->stepsizeboxchanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->filterboxchanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->removeButtonClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData AdjustFilterSettings::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AdjustFilterSettings::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AdjustFilterSettings,
      qt_meta_data_AdjustFilterSettings, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AdjustFilterSettings::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AdjustFilterSettings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AdjustFilterSettings::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AdjustFilterSettings))
        return static_cast<void*>(const_cast< AdjustFilterSettings*>(this));
    return QObject::qt_metacast(_clname);
}

int AdjustFilterSettings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
