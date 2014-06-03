/****************************************************************************
** Meta object code from reading C++ file 'test_layer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "test_layer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'test_layer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TestLayer_t {
    QByteArrayData data[13];
    char stringdata[235];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_TestLayer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_TestLayer_t qt_meta_stringdata_TestLayer = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 12),
QT_MOC_LITERAL(2, 23, 0),
QT_MOC_LITERAL(3, 24, 15),
QT_MOC_LITERAL(4, 40, 9),
QT_MOC_LITERAL(5, 50, 13),
QT_MOC_LITERAL(6, 64, 20),
QT_MOC_LITERAL(7, 85, 18),
QT_MOC_LITERAL(8, 104, 23),
QT_MOC_LITERAL(9, 128, 25),
QT_MOC_LITERAL(10, 154, 25),
QT_MOC_LITERAL(11, 180, 28),
QT_MOC_LITERAL(12, 209, 24)
    },
    "TestLayer\0initTestCase\0\0cleanupTestCase\0"
    "testCase1\0testLayerType\0testAddNewKeyFrameAt\0"
    "testRemoveKeyFrame\0testGetMaxFramePosition\0"
    "testGetFirstFramePosition\0"
    "testHasKeyFrameAtPosition\0"
    "testPreviousKeyFramePosition\0"
    "testNextKeyFramePosition\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TestLayer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x08,
       3,    0,   70,    2, 0x08,
       4,    0,   71,    2, 0x08,
       5,    0,   72,    2, 0x08,
       6,    0,   73,    2, 0x08,
       7,    0,   74,    2, 0x08,
       8,    0,   75,    2, 0x08,
       9,    0,   76,    2, 0x08,
      10,    0,   77,    2, 0x08,
      11,    0,   78,    2, 0x08,
      12,    0,   79,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TestLayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TestLayer *_t = static_cast<TestLayer *>(_o);
        switch (_id) {
        case 0: _t->initTestCase(); break;
        case 1: _t->cleanupTestCase(); break;
        case 2: _t->testCase1(); break;
        case 3: _t->testLayerType(); break;
        case 4: _t->testAddNewKeyFrameAt(); break;
        case 5: _t->testRemoveKeyFrame(); break;
        case 6: _t->testGetMaxFramePosition(); break;
        case 7: _t->testGetFirstFramePosition(); break;
        case 8: _t->testHasKeyFrameAtPosition(); break;
        case 9: _t->testPreviousKeyFramePosition(); break;
        case 10: _t->testNextKeyFramePosition(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject TestLayer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_TestLayer.data,
      qt_meta_data_TestLayer,  qt_static_metacall, 0, 0}
};


const QMetaObject *TestLayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TestLayer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TestLayer.stringdata))
        return static_cast<void*>(const_cast< TestLayer*>(this));
    return QObject::qt_metacast(_clname);
}

int TestLayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
