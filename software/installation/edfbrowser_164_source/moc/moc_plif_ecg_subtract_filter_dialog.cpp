/****************************************************************************
** Meta object code from reading C++ file 'plif_ecg_subtract_filter_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../plif_ecg_subtract_filter_dialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plif_ecg_subtract_filter_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_PLIF_ECG_filter_dialog[] = {

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
      27,   26,   26,   26, 0x08,
      48,   26,   26,   26, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_PLIF_ECG_filter_dialog[] = {
    "UI_PLIF_ECG_filter_dialog\0\0"
    "ApplyButtonClicked()\0helpbuttonpressed()\0"
};

void UI_PLIF_ECG_filter_dialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_PLIF_ECG_filter_dialog *_t = static_cast<UI_PLIF_ECG_filter_dialog *>(_o);
        switch (_id) {
        case 0: _t->ApplyButtonClicked(); break;
        case 1: _t->helpbuttonpressed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData UI_PLIF_ECG_filter_dialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_PLIF_ECG_filter_dialog::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UI_PLIF_ECG_filter_dialog,
      qt_meta_data_UI_PLIF_ECG_filter_dialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_PLIF_ECG_filter_dialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_PLIF_ECG_filter_dialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_PLIF_ECG_filter_dialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_PLIF_ECG_filter_dialog))
        return static_cast<void*>(const_cast< UI_PLIF_ECG_filter_dialog*>(this));
    return QObject::qt_metacast(_clname);
}

int UI_PLIF_ECG_filter_dialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
