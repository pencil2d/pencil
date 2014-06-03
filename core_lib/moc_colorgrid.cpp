/****************************************************************************
** Meta object code from reading C++ file 'colorgrid.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interface/colorgrid.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'colorgrid.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ColorGrid_t {
    QByteArrayData data[5];
    char stringdata[45];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_ColorGrid_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_ColorGrid_t qt_meta_stringdata_ColorGrid = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 11),
QT_MOC_LITERAL(2, 22, 0),
QT_MOC_LITERAL(3, 23, 11),
QT_MOC_LITERAL(4, 35, 8)
    },
    "ColorGrid\0colorDroped\0\0colorPicked\0"
    "setColor\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ColorGrid[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x05,
       1,    1,   39,    2, 0x05,
       3,    2,   42,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       4,    2,   47,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QColor,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QColor,    2,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QColor,    2,    2,

       0        // eod
};

void ColorGrid::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ColorGrid *_t = static_cast<ColorGrid *>(_o);
        switch (_id) {
        case 0: _t->colorDroped((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 1: _t->colorDroped((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 2: _t->colorPicked((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 3: _t->setColor((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ColorGrid::*_t)(const int & , const QColor & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ColorGrid::colorDroped)) {
                *result = 0;
            }
        }
        {
            typedef void (ColorGrid::*_t)(const int & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ColorGrid::colorDroped)) {
                *result = 1;
            }
        }
        {
            typedef void (ColorGrid::*_t)(const int & , const QColor & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ColorGrid::colorPicked)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject ColorGrid::staticMetaObject = {
    { &QScrollArea::staticMetaObject, qt_meta_stringdata_ColorGrid.data,
      qt_meta_data_ColorGrid,  qt_static_metacall, 0, 0}
};


const QMetaObject *ColorGrid::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ColorGrid::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ColorGrid.stringdata))
        return static_cast<void*>(const_cast< ColorGrid*>(this));
    return QScrollArea::qt_metacast(_clname);
}

int ColorGrid::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ColorGrid::colorDroped(const int & _t1, const QColor & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ColorGrid::colorDroped(const int & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ColorGrid::colorPicked(const int & _t1, const QColor & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
