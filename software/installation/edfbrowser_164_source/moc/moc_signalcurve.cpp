/****************************************************************************
** Meta object code from reading C++ file 'signalcurve.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../signalcurve.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'signalcurve.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SignalCurve[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      36,   12,   12,   12, 0x05,
      55,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      72,   12,   12,   12, 0x08,
      88,   12,   12,   12, 0x08,
     110,   12,   12,   12, 0x08,
     125,   12,   12,   12, 0x08,
     142,   12,   12,   12, 0x08,
     161,   12,   12,   12, 0x08,
     178,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SignalCurve[] = {
    "SignalCurve\0\0extra_button_clicked()\0"
    "dashBoardClicked()\0markerHasMoved()\0"
    "exec_sidemenu()\0print_to_postscript()\0"
    "print_to_pdf()\0print_to_image()\0"
    "print_to_printer()\0print_to_ascii()\0"
    "send_button_event()\0"
};

void SignalCurve::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SignalCurve *_t = static_cast<SignalCurve *>(_o);
        switch (_id) {
        case 0: _t->extra_button_clicked(); break;
        case 1: _t->dashBoardClicked(); break;
        case 2: _t->markerHasMoved(); break;
        case 3: _t->exec_sidemenu(); break;
        case 4: _t->print_to_postscript(); break;
        case 5: _t->print_to_pdf(); break;
        case 6: _t->print_to_image(); break;
        case 7: _t->print_to_printer(); break;
        case 8: _t->print_to_ascii(); break;
        case 9: _t->send_button_event(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SignalCurve::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SignalCurve::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SignalCurve,
      qt_meta_data_SignalCurve, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SignalCurve::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SignalCurve::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SignalCurve::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SignalCurve))
        return static_cast<void*>(const_cast< SignalCurve*>(this));
    return QWidget::qt_metacast(_clname);
}

int SignalCurve::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void SignalCurve::extra_button_clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SignalCurve::dashBoardClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void SignalCurve::markerHasMoved()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
