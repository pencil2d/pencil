/****************************************************************************
** Meta object code from reading C++ file 'colorpalettewidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interface/colorpalettewidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'colorpalettewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ColorPaletteWidget_t {
    QByteArrayData data[14];
    char stringdata[237];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_ColorPaletteWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_ColorPaletteWidget_t qt_meta_stringdata_ColorPaletteWidget = {
    {
QT_MOC_LITERAL(0, 0, 18),
QT_MOC_LITERAL(1, 19, 12),
QT_MOC_LITERAL(2, 32, 0),
QT_MOC_LITERAL(3, 33, 18),
QT_MOC_LITERAL(4, 52, 17),
QT_MOC_LITERAL(5, 70, 8),
QT_MOC_LITERAL(6, 79, 16),
QT_MOC_LITERAL(7, 96, 15),
QT_MOC_LITERAL(8, 112, 27),
QT_MOC_LITERAL(9, 140, 16),
QT_MOC_LITERAL(10, 157, 18),
QT_MOC_LITERAL(11, 176, 16),
QT_MOC_LITERAL(12, 193, 19),
QT_MOC_LITERAL(13, 213, 22)
    },
    "ColorPaletteWidget\0colorChanged\0\0"
    "colorNumberChanged\0selectColorNumber\0"
    "setColor\0refreshColorList\0updateItemColor\0"
    "colorListCurrentItemChanged\0"
    "QListWidgetItem*\0clickColorListItem\0"
    "changeColourName\0clickAddColorButton\0"
    "clickRemoveColorButton\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ColorPaletteWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x05,
       3,    1,   72,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       4,    1,   75,    2, 0x0a,
       5,    1,   78,    2, 0x0a,
       6,    0,   81,    2, 0x0a,
       7,    2,   82,    2, 0x08,
       8,    2,   87,    2, 0x08,
      10,    1,   92,    2, 0x08,
      11,    1,   95,    2, 0x08,
      12,    0,   98,    2, 0x08,
      13,    0,   99,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::QColor,    2,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QColor,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QColor,    2,    2,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 9,    2,    2,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ColorPaletteWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ColorPaletteWidget *_t = static_cast<ColorPaletteWidget *>(_o);
        switch (_id) {
        case 0: _t->colorChanged((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 1: _t->colorNumberChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->selectColorNumber((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setColor((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 4: _t->refreshColorList(); break;
        case 5: _t->updateItemColor((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 6: _t->colorListCurrentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 7: _t->clickColorListItem((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 8: _t->changeColourName((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 9: _t->clickAddColorButton(); break;
        case 10: _t->clickRemoveColorButton(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ColorPaletteWidget::*_t)(QColor );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ColorPaletteWidget::colorChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (ColorPaletteWidget::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ColorPaletteWidget::colorNumberChanged)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject ColorPaletteWidget::staticMetaObject = {
    { &BaseDockWidget::staticMetaObject, qt_meta_stringdata_ColorPaletteWidget.data,
      qt_meta_data_ColorPaletteWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *ColorPaletteWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ColorPaletteWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ColorPaletteWidget.stringdata))
        return static_cast<void*>(const_cast< ColorPaletteWidget*>(this));
    return BaseDockWidget::qt_metacast(_clname);
}

int ColorPaletteWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseDockWidget::qt_metacall(_c, _id, _a);
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
void ColorPaletteWidget::colorChanged(QColor _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ColorPaletteWidget::colorNumberChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
