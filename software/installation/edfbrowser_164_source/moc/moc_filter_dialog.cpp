/****************************************************************************
** Meta object code from reading C++ file 'filter_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../filter_dialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filter_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_FilterDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      38,   16,   16,   16, 0x08,
      71,   16,   16,   16, 0x08,
      97,   16,   16,   16, 0x08,
     128,   16,   16,   16, 0x08,
     160,   16,   16,   16, 0x08,
     190,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_FilterDialog[] = {
    "UI_FilterDialog\0\0ApplyButtonClicked()\0"
    "frequencyboxvaluechanged(double)\0"
    "orderboxvaluechanged(int)\0"
    "filtertypeboxvaluechanged(int)\0"
    "filtermodelboxvaluechanged(int)\0"
    "rippleboxvaluechanged(double)\0"
    "freq2boxvaluechanged(double)\0"
};

void UI_FilterDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_FilterDialog *_t = static_cast<UI_FilterDialog *>(_o);
        switch (_id) {
        case 0: _t->ApplyButtonClicked(); break;
        case 1: _t->frequencyboxvaluechanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->orderboxvaluechanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->filtertypeboxvaluechanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->filtermodelboxvaluechanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->rippleboxvaluechanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->freq2boxvaluechanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_FilterDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_FilterDialog::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UI_FilterDialog,
      qt_meta_data_UI_FilterDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_FilterDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_FilterDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_FilterDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_FilterDialog))
        return static_cast<void*>(const_cast< UI_FilterDialog*>(this));
    return QObject::qt_metacast(_clname);
}

int UI_FilterDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
