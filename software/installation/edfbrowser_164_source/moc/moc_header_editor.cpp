/****************************************************************************
** Meta object code from reading C++ file 'header_editor.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../header_editor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'header_editor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_headerEditorWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x08,
      35,   22,   22,   22, 0x08,
      46,   22,   22,   22, 0x08,
      60,   22,   22,   22, 0x08,
      85,   22,   22,   22, 0x08,
     120,   22,  116,   22, 0x08,
     155,   22,  116,   22, 0x08,
     195,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_headerEditorWindow[] = {
    "UI_headerEditorWindow\0\0open_file()\0"
    "save_hdr()\0read_header()\0"
    "closeEvent(QCloseEvent*)\0"
    "calculate_chars_left_name(int)\0int\0"
    "calculate_chars_left_name(QString)\0"
    "calculate_chars_left_recording(QString)\0"
    "helpbuttonpressed()\0"
};

void UI_headerEditorWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_headerEditorWindow *_t = static_cast<UI_headerEditorWindow *>(_o);
        switch (_id) {
        case 0: _t->open_file(); break;
        case 1: _t->save_hdr(); break;
        case 2: _t->read_header(); break;
        case 3: _t->closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 4: _t->calculate_chars_left_name((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: { int _r = _t->calculate_chars_left_name((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 6: { int _r = _t->calculate_chars_left_recording((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 7: _t->helpbuttonpressed(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_headerEditorWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_headerEditorWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_UI_headerEditorWindow,
      qt_meta_data_UI_headerEditorWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_headerEditorWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_headerEditorWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_headerEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_headerEditorWindow))
        return static_cast<void*>(const_cast< UI_headerEditorWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int UI_headerEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
