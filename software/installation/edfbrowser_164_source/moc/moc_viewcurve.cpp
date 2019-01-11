/****************************************************************************
** Meta object code from reading C++ file 'viewcurve.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../viewcurve.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewcurve.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ViewCurve[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,
      30,   10,   10,   10, 0x0a,
      52,   10,   10,   10, 0x0a,
      69,   67,   10,   10, 0x0a,
      93,   10,   10,   10, 0x0a,
     112,   10,   10,   10, 0x09,
     126,   10,   10,   10, 0x09,
     144,   10,   10,   10, 0x09,
     168,   10,   10,   10, 0x09,
     193,   10,   10,   10, 0x09,
     214,   10,   10,   10, 0x09,
     235,   10,   10,   10, 0x09,
     249,   10,   10,   10, 0x09,
     267,   10,   10,   10, 0x09,
     284,   10,   10,   10, 0x09,
     302,   10,   10,   10, 0x09,
     323,   10,   10,   10, 0x09,
     342,   10,   10,   10, 0x09,
     360,   10,   10,   10, 0x09,
     375,   10,   10,   10, 0x09,
     405,   10,   10,   10, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ViewCurve[] = {
    "ViewCurve\0\0exec_sidemenu(int)\0"
    "print_to_postscript()\0print_to_pdf()\0"
    ",\0print_to_image(int,int)\0print_to_printer()\0"
    "RulerButton()\0FittopaneButton()\0"
    "ScaleBoxChanged(double)\0"
    "ScaleBox2Changed(double)\0RemovefilterButton()\0"
    "RemovesignalButton()\0ColorButton()\0"
    "CrosshairButton()\0FreqSpecButton()\0"
    "Z_scoringButton()\0AdjustFilterButton()\0"
    "StatisticsButton()\0ECGdetectButton()\0"
    "signalInvert()\0strip_types_from_label(char*)\0"
    "sidemenu_close()\0"
};

void ViewCurve::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ViewCurve *_t = static_cast<ViewCurve *>(_o);
        switch (_id) {
        case 0: _t->exec_sidemenu((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->print_to_postscript(); break;
        case 2: _t->print_to_pdf(); break;
        case 3: _t->print_to_image((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->print_to_printer(); break;
        case 5: _t->RulerButton(); break;
        case 6: _t->FittopaneButton(); break;
        case 7: _t->ScaleBoxChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->ScaleBox2Changed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->RemovefilterButton(); break;
        case 10: _t->RemovesignalButton(); break;
        case 11: _t->ColorButton(); break;
        case 12: _t->CrosshairButton(); break;
        case 13: _t->FreqSpecButton(); break;
        case 14: _t->Z_scoringButton(); break;
        case 15: _t->AdjustFilterButton(); break;
        case 16: _t->StatisticsButton(); break;
        case 17: _t->ECGdetectButton(); break;
        case 18: _t->signalInvert(); break;
        case 19: _t->strip_types_from_label((*reinterpret_cast< char*(*)>(_a[1]))); break;
        case 20: _t->sidemenu_close(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ViewCurve::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ViewCurve::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ViewCurve,
      qt_meta_data_ViewCurve, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ViewCurve::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ViewCurve::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ViewCurve::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ViewCurve))
        return static_cast<void*>(const_cast< ViewCurve*>(this));
    return QWidget::qt_metacast(_clname);
}

int ViewCurve::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
