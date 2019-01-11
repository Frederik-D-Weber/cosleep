/****************************************************************************
** Meta object code from reading C++ file 'averager_curve_wnd.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../averager_curve_wnd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'averager_curve_wnd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UI_AverageCurveWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x08,
      40,   22,   22,   22, 0x08,
      81,   22,   22,   22, 0x08,
      94,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UI_AverageCurveWindow[] = {
    "UI_AverageCurveWindow\0\0sliderMoved(int)\0"
    "averager_curve_dialogDestroyed(QObject*)\0"
    "export_edf()\0update_flywheel(int)\0"
};

void UI_AverageCurveWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        UI_AverageCurveWindow *_t = static_cast<UI_AverageCurveWindow *>(_o);
        switch (_id) {
        case 0: _t->sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->averager_curve_dialogDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 2: _t->export_edf(); break;
        case 3: _t->update_flywheel((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData UI_AverageCurveWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject UI_AverageCurveWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_UI_AverageCurveWindow,
      qt_meta_data_UI_AverageCurveWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UI_AverageCurveWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UI_AverageCurveWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UI_AverageCurveWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UI_AverageCurveWindow))
        return static_cast<void*>(const_cast< UI_AverageCurveWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int UI_AverageCurveWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
