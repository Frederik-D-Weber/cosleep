/****************************************************************************
** Meta object code from reading C++ file 'z_score_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../z_score_dialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'z_score_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_ZScoreWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      38,   16,   16,   16, 0x08,
      69,   16,   16,   16, 0x08,
      89,   16,   16,   16, 0x08,
     121,   16,   16,   16, 0x08,
     147,   16,   16,   16, 0x08,
     170,   16,   16,   16, 0x08,
     185,   16,   16,   16, 0x08,
     209,   16,   16,   16, 0x08,
     228,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_ZScoreWindow[] = {
    "UI_ZScoreWindow\0\0startButtonClicked()\0"
    "get_annotationsButtonClicked()\0"
    "jumpButtonClicked()\0ZscoreDialogDestroyed(QObject*)\0"
    "RadioButtonsClicked(bool)\0"
    "defaultButtonClicked()\0markersMoved()\0"
    "addTraceButtonClicked()\0shift_page_right()\0"
    "shift_page_left()\0"
};

void UI_ZScoreWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_ZScoreWindow *_t = static_cast<UI_ZScoreWindow *>(_o);
        switch (_id) {
        case 0: _t->startButtonClicked(); break;
        case 1: _t->get_annotationsButtonClicked(); break;
        case 2: _t->jumpButtonClicked(); break;
        case 3: _t->ZscoreDialogDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 4: _t->RadioButtonsClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->defaultButtonClicked(); break;
        case 6: _t->markersMoved(); break;
        case 7: _t->addTraceButtonClicked(); break;
        case 8: _t->shift_page_right(); break;
        case 9: _t->shift_page_left(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_ZScoreWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_ZScoreWindow::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UI_ZScoreWindow,
      qt_meta_data_UI_ZScoreWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_ZScoreWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_ZScoreWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_ZScoreWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_ZScoreWindow))
        return static_cast<void*>(const_cast< UI_ZScoreWindow*>(this));
    return QObject::qt_metacast(_clname);
}

int UI_ZScoreWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
