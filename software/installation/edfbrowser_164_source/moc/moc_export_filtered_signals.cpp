/****************************************************************************
** Meta object code from reading C++ file 'export_filtered_signals.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../export_filtered_signals.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'export_filtered_signals.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_ExportFilteredSignalsWindow[] = {

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
      32,   31,   31,   31, 0x08,
      51,   31,   31,   31, 0x08,
      65,   31,   31,   31, 0x08,
      86,   31,   31,   31, 0x08,
     107,   31,   31,   31, 0x08,
     133,   31,   31,   31, 0x08,
     159,   31,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_ExportFilteredSignalsWindow[] = {
    "UI_ExportFilteredSignalsWindow\0\0"
    "SelectFileButton()\0StartExport()\0"
    "spinBox1changed(int)\0spinBox2changed(int)\0"
    "radioButton1Toggled(bool)\0"
    "radioButton2Toggled(bool)\0"
    "populate_tree_view()\0"
};

void UI_ExportFilteredSignalsWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_ExportFilteredSignalsWindow *_t = static_cast<UI_ExportFilteredSignalsWindow *>(_o);
        switch (_id) {
        case 0: _t->SelectFileButton(); break;
        case 1: _t->StartExport(); break;
        case 2: _t->spinBox1changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->spinBox2changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->radioButton1Toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->radioButton2Toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->populate_tree_view(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_ExportFilteredSignalsWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_ExportFilteredSignalsWindow::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UI_ExportFilteredSignalsWindow,
      qt_meta_data_UI_ExportFilteredSignalsWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_ExportFilteredSignalsWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_ExportFilteredSignalsWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_ExportFilteredSignalsWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_ExportFilteredSignalsWindow))
        return static_cast<void*>(const_cast< UI_ExportFilteredSignalsWindow*>(this));
    return QObject::qt_metacast(_clname);
}

int UI_ExportFilteredSignalsWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
