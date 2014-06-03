/****************************************************************************
** Meta object code from reading C++ file 'timecontrols.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interface/timecontrols.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'timecontrols.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TimeControls_t {
    QByteArrayData data[16];
    char stringdata[214];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_TimeControls_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_TimeControls_t qt_meta_stringdata_TimeControls = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 9),
QT_MOC_LITERAL(2, 23, 0),
QT_MOC_LITERAL(3, 24, 18),
QT_MOC_LITERAL(4, 43, 20),
QT_MOC_LITERAL(5, 64, 9),
QT_MOC_LITERAL(6, 74, 16),
QT_MOC_LITERAL(7, 91, 10),
QT_MOC_LITERAL(8, 102, 8),
QT_MOC_LITERAL(9, 111, 11),
QT_MOC_LITERAL(10, 123, 18),
QT_MOC_LITERAL(11, 142, 14),
QT_MOC_LITERAL(12, 157, 12),
QT_MOC_LITERAL(13, 170, 13),
QT_MOC_LITERAL(14, 184, 10),
QT_MOC_LITERAL(15, 195, 17)
    },
    "TimeControls\0playClick\0\0clickGotoEndButton\0"
    "clickGotoStartButton\0loopClick\0"
    "loopControlClick\0soundClick\0fpsClick\0"
    "loopToggled\0loopControlToggled\0"
    "loopStartClick\0loopEndClick\0updateButtons\0"
    "toggleLoop\0toggleLoopControl\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TimeControls[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x05,
       3,    0,   85,    2, 0x05,
       4,    0,   86,    2, 0x05,
       5,    1,   87,    2, 0x05,
       6,    1,   90,    2, 0x05,
       7,    1,   93,    2, 0x05,
       8,    1,   96,    2, 0x05,
       9,    1,   99,    2, 0x05,
      10,    1,  102,    2, 0x05,
      11,    1,  105,    2, 0x05,
      12,    1,  108,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
      13,    1,  111,    2, 0x0a,
      14,    1,  114,    2, 0x0a,
      15,    1,  117,    2, 0x0a,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void TimeControls::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TimeControls *_t = static_cast<TimeControls *>(_o);
        switch (_id) {
        case 0: _t->playClick(); break;
        case 1: _t->clickGotoEndButton(); break;
        case 2: _t->clickGotoStartButton(); break;
        case 3: _t->loopClick((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->loopControlClick((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->soundClick((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->fpsClick((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->loopToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->loopControlToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->loopStartClick((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->loopEndClick((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->updateButtons((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->toggleLoop((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->toggleLoopControl((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TimeControls::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeControls::playClick)) {
                *result = 0;
            }
        }
        {
            typedef void (TimeControls::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeControls::clickGotoEndButton)) {
                *result = 1;
            }
        }
        {
            typedef void (TimeControls::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeControls::clickGotoStartButton)) {
                *result = 2;
            }
        }
        {
            typedef void (TimeControls::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeControls::loopClick)) {
                *result = 3;
            }
        }
        {
            typedef void (TimeControls::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeControls::loopControlClick)) {
                *result = 4;
            }
        }
        {
            typedef void (TimeControls::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeControls::soundClick)) {
                *result = 5;
            }
        }
        {
            typedef void (TimeControls::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeControls::fpsClick)) {
                *result = 6;
            }
        }
        {
            typedef void (TimeControls::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeControls::loopToggled)) {
                *result = 7;
            }
        }
        {
            typedef void (TimeControls::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeControls::loopControlToggled)) {
                *result = 8;
            }
        }
        {
            typedef void (TimeControls::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeControls::loopStartClick)) {
                *result = 9;
            }
        }
        {
            typedef void (TimeControls::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeControls::loopEndClick)) {
                *result = 10;
            }
        }
    }
}

const QMetaObject TimeControls::staticMetaObject = {
    { &QToolBar::staticMetaObject, qt_meta_stringdata_TimeControls.data,
      qt_meta_data_TimeControls,  qt_static_metacall, 0, 0}
};


const QMetaObject *TimeControls::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TimeControls::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TimeControls.stringdata))
        return static_cast<void*>(const_cast< TimeControls*>(this));
    return QToolBar::qt_metacast(_clname);
}

int TimeControls::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void TimeControls::playClick()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void TimeControls::clickGotoEndButton()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void TimeControls::clickGotoStartButton()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void TimeControls::loopClick(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void TimeControls::loopControlClick(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void TimeControls::soundClick(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void TimeControls::fpsClick(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void TimeControls::loopToggled(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void TimeControls::loopControlToggled(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void TimeControls::loopStartClick(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void TimeControls::loopEndClick(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_END_MOC_NAMESPACE
