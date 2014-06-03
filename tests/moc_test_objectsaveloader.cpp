/****************************************************************************
** Meta object code from reading C++ file 'test_objectsaveloader.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "test_objectsaveloader.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'test_objectsaveloader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TestObjectSaveLoader_t {
    QByteArrayData data[7];
    char stringdata[117];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_TestObjectSaveLoader_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_TestObjectSaveLoader_t qt_meta_stringdata_TestObjectSaveLoader = {
    {
QT_MOC_LITERAL(0, 0, 20),
QT_MOC_LITERAL(1, 21, 9),
QT_MOC_LITERAL(2, 31, 0),
QT_MOC_LITERAL(3, 32, 16),
QT_MOC_LITERAL(4, 49, 14),
QT_MOC_LITERAL(5, 64, 25),
QT_MOC_LITERAL(6, 90, 25)
    },
    "TestObjectSaveLoader\0testCase1\0\0"
    "testNotExistFile\0testInvalidXML\0"
    "testInvalidPencilDocument\0"
    "testMinimalPencilDocument\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TestObjectSaveLoader[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08,
       3,    0,   40,    2, 0x08,
       4,    0,   41,    2, 0x08,
       5,    0,   42,    2, 0x08,
       6,    0,   43,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TestObjectSaveLoader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TestObjectSaveLoader *_t = static_cast<TestObjectSaveLoader *>(_o);
        switch (_id) {
        case 0: _t->testCase1(); break;
        case 1: _t->testNotExistFile(); break;
        case 2: _t->testInvalidXML(); break;
        case 3: _t->testInvalidPencilDocument(); break;
        case 4: _t->testMinimalPencilDocument(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject TestObjectSaveLoader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_TestObjectSaveLoader.data,
      qt_meta_data_TestObjectSaveLoader,  qt_static_metacall, 0, 0}
};


const QMetaObject *TestObjectSaveLoader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TestObjectSaveLoader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TestObjectSaveLoader.stringdata))
        return static_cast<void*>(const_cast< TestObjectSaveLoader*>(this));
    return QObject::qt_metacast(_clname);
}

int TestObjectSaveLoader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
