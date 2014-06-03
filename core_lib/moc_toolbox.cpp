/****************************************************************************
** Meta object code from reading C++ file 'toolbox.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interface/toolbox.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'toolbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ToolBoxWidget_t {
    QByteArrayData data[14];
    char stringdata[132];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_ToolBoxWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_ToolBoxWidget_t qt_meta_stringdata_ToolBoxWidget = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 18),
QT_MOC_LITERAL(2, 33, 0),
QT_MOC_LITERAL(3, 34, 8),
QT_MOC_LITERAL(4, 43, 8),
QT_MOC_LITERAL(5, 52, 8),
QT_MOC_LITERAL(6, 61, 6),
QT_MOC_LITERAL(7, 68, 5),
QT_MOC_LITERAL(8, 74, 6),
QT_MOC_LITERAL(9, 81, 10),
QT_MOC_LITERAL(10, 92, 8),
QT_MOC_LITERAL(11, 101, 12),
QT_MOC_LITERAL(12, 114, 7),
QT_MOC_LITERAL(13, 122, 8)
    },
    "ToolBoxWidget\0clearButtonClicked\0\0"
    "pencilOn\0eraserOn\0selectOn\0moveOn\0"
    "penOn\0handOn\0polylineOn\0bucketOn\0"
    "eyedropperOn\0brushOn\0smudgeOn\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ToolBoxWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       3,    0,   75,    2, 0x0a,
       4,    0,   76,    2, 0x0a,
       5,    0,   77,    2, 0x0a,
       6,    0,   78,    2, 0x0a,
       7,    0,   79,    2, 0x0a,
       8,    0,   80,    2, 0x0a,
       9,    0,   81,    2, 0x0a,
      10,    0,   82,    2, 0x0a,
      11,    0,   83,    2, 0x0a,
      12,    0,   84,    2, 0x0a,
      13,    0,   85,    2, 0x0a,

 // signals: parameters
    QMetaType::Void,

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

void ToolBoxWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ToolBoxWidget *_t = static_cast<ToolBoxWidget *>(_o);
        switch (_id) {
        case 0: _t->clearButtonClicked(); break;
        case 1: _t->pencilOn(); break;
        case 2: _t->eraserOn(); break;
        case 3: _t->selectOn(); break;
        case 4: _t->moveOn(); break;
        case 5: _t->penOn(); break;
        case 6: _t->handOn(); break;
        case 7: _t->polylineOn(); break;
        case 8: _t->bucketOn(); break;
        case 9: _t->eyedropperOn(); break;
        case 10: _t->brushOn(); break;
        case 11: _t->smudgeOn(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ToolBoxWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ToolBoxWidget::clearButtonClicked)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ToolBoxWidget::staticMetaObject = {
    { &BaseDockWidget::staticMetaObject, qt_meta_stringdata_ToolBoxWidget.data,
      qt_meta_data_ToolBoxWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *ToolBoxWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ToolBoxWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ToolBoxWidget.stringdata))
        return static_cast<void*>(const_cast< ToolBoxWidget*>(this));
    return BaseDockWidget::qt_metacast(_clname);
}

int ToolBoxWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void ToolBoxWidget::clearButtonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
