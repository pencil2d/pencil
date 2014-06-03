/****************************************************************************
** Meta object code from reading C++ file 'timelinecells.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interface/timelinecells.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'timelinecells.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TimeLineCells_t {
    QByteArrayData data[15];
    char stringdata[178];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_TimeLineCells_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_TimeLineCells_t qt_meta_stringdata_TimeLineCells = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 11),
QT_MOC_LITERAL(2, 26, 0),
QT_MOC_LITERAL(3, 27, 13),
QT_MOC_LITERAL(4, 41, 11),
QT_MOC_LITERAL(5, 53, 11),
QT_MOC_LITERAL(6, 65, 12),
QT_MOC_LITERAL(7, 78, 15),
QT_MOC_LITERAL(8, 94, 14),
QT_MOC_LITERAL(9, 109, 11),
QT_MOC_LITERAL(10, 121, 11),
QT_MOC_LITERAL(11, 133, 13),
QT_MOC_LITERAL(12, 147, 13),
QT_MOC_LITERAL(13, 161, 13),
QT_MOC_LITERAL(14, 175, 1)
    },
    "TimeLineCells\0mouseMovedY\0\0updateContent\0"
    "updateFrame\0frameNumber\0lengthChange\0"
    "frameSizeChange\0fontSizeChange\0"
    "scrubChange\0labelChange\0hScrollChange\0"
    "vScrollChange\0setMouseMoveY\0x\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TimeLineCells[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       3,    0,   72,    2, 0x0a,
       4,    1,   73,    2, 0x0a,
       6,    1,   76,    2, 0x0a,
       7,    1,   79,    2, 0x0a,
       8,    1,   82,    2, 0x0a,
       9,    1,   85,    2, 0x0a,
      10,    1,   88,    2, 0x0a,
      11,    1,   91,    2, 0x0a,
      12,    1,   94,    2, 0x0a,
      13,    1,   97,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   14,

       0        // eod
};

void TimeLineCells::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TimeLineCells *_t = static_cast<TimeLineCells *>(_o);
        switch (_id) {
        case 0: _t->mouseMovedY((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateContent(); break;
        case 2: _t->updateFrame((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->lengthChange((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->frameSizeChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->fontSizeChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->scrubChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->labelChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->hScrollChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->vScrollChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->setMouseMoveY((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TimeLineCells::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimeLineCells::mouseMovedY)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject TimeLineCells::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TimeLineCells.data,
      qt_meta_data_TimeLineCells,  qt_static_metacall, 0, 0}
};


const QMetaObject *TimeLineCells::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TimeLineCells::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TimeLineCells.stringdata))
        return static_cast<void*>(const_cast< TimeLineCells*>(this));
    return QWidget::qt_metacast(_clname);
}

int TimeLineCells::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void TimeLineCells::mouseMovedY(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
