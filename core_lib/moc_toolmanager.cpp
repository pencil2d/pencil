/****************************************************************************
** Meta object code from reading C++ file 'toolmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "managers/toolmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'toolmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ToolManager_t {
    QByteArrayData data[16];
    char stringdata[253];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_ToolManager_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_ToolManager_t qt_meta_stringdata_ToolManager = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 19),
QT_MOC_LITERAL(2, 32, 0),
QT_MOC_LITERAL(3, 33, 21),
QT_MOC_LITERAL(4, 55, 25),
QT_MOC_LITERAL(5, 81, 27),
QT_MOC_LITERAL(6, 109, 22),
QT_MOC_LITERAL(7, 132, 11),
QT_MOC_LITERAL(8, 144, 8),
QT_MOC_LITERAL(9, 153, 19),
QT_MOC_LITERAL(10, 173, 13),
QT_MOC_LITERAL(11, 187, 8),
QT_MOC_LITERAL(12, 196, 10),
QT_MOC_LITERAL(13, 207, 15),
QT_MOC_LITERAL(14, 223, 16),
QT_MOC_LITERAL(15, 240, 11)
    },
    "ToolManager\0penWidthValueChange\0\0"
    "penFeatherValueChange\0penInvisiblityValueChange\0"
    "penPreserveAlphaValueChange\0"
    "penPressureValueChange\0toolChanged\0"
    "ToolType\0toolPropertyChanged\0resetAllTools\0"
    "setWidth\0setFeather\0setInvisibility\0"
    "setPreserveAlpha\0setPressure\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ToolManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x05,
       3,    1,   82,    2, 0x05,
       4,    1,   85,    2, 0x05,
       5,    1,   88,    2, 0x05,
       6,    1,   91,    2, 0x05,
       7,    1,   94,    2, 0x05,
       9,    0,   97,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
      10,    0,   98,    2, 0x0a,
      11,    1,   99,    2, 0x0a,
      12,    1,  102,    2, 0x0a,
      13,    1,  105,    2, 0x0a,
      14,    1,  108,    2, 0x0a,
      15,    1,  111,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 8,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void ToolManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ToolManager *_t = static_cast<ToolManager *>(_o);
        switch (_id) {
        case 0: _t->penWidthValueChange((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 1: _t->penFeatherValueChange((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 2: _t->penInvisiblityValueChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->penPreserveAlphaValueChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->penPressureValueChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->toolChanged((*reinterpret_cast< ToolType(*)>(_a[1]))); break;
        case 6: _t->toolPropertyChanged(); break;
        case 7: _t->resetAllTools(); break;
        case 8: _t->setWidth((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 9: _t->setFeather((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 10: _t->setInvisibility((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->setPreserveAlpha((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->setPressure((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ToolManager::*_t)(float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ToolManager::penWidthValueChange)) {
                *result = 0;
            }
        }
        {
            typedef void (ToolManager::*_t)(float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ToolManager::penFeatherValueChange)) {
                *result = 1;
            }
        }
        {
            typedef void (ToolManager::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ToolManager::penInvisiblityValueChange)) {
                *result = 2;
            }
        }
        {
            typedef void (ToolManager::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ToolManager::penPreserveAlphaValueChange)) {
                *result = 3;
            }
        }
        {
            typedef void (ToolManager::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ToolManager::penPressureValueChange)) {
                *result = 4;
            }
        }
        {
            typedef void (ToolManager::*_t)(ToolType );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ToolManager::toolChanged)) {
                *result = 5;
            }
        }
        {
            typedef void (ToolManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ToolManager::toolPropertyChanged)) {
                *result = 6;
            }
        }
    }
}

const QMetaObject ToolManager::staticMetaObject = {
    { &BaseManager::staticMetaObject, qt_meta_stringdata_ToolManager.data,
      qt_meta_data_ToolManager,  qt_static_metacall, 0, 0}
};


const QMetaObject *ToolManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ToolManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ToolManager.stringdata))
        return static_cast<void*>(const_cast< ToolManager*>(this));
    return BaseManager::qt_metacast(_clname);
}

int ToolManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseManager::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void ToolManager::penWidthValueChange(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ToolManager::penFeatherValueChange(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ToolManager::penInvisiblityValueChange(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ToolManager::penPreserveAlphaValueChange(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ToolManager::penPressureValueChange(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ToolManager::toolChanged(ToolType _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ToolManager::toolPropertyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}
QT_END_MOC_NAMESPACE
