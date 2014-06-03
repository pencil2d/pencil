/****************************************************************************
** Meta object code from reading C++ file 'shortcutspage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interface/shortcutspage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'shortcutspage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ShortcutsPage_t {
    QByteArrayData data[6];
    char stringdata[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_ShortcutsPage_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_ShortcutsPage_t qt_meta_stringdata_ShortcutsPage = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 16),
QT_MOC_LITERAL(2, 31, 0),
QT_MOC_LITERAL(3, 32, 25),
QT_MOC_LITERAL(4, 58, 29),
QT_MOC_LITERAL(5, 88, 18)
    },
    "ShortcutsPage\0tableItemClicked\0\0"
    "keyCapLineEditTextChanged\0"
    "restoreShortcutsButtonClicked\0"
    "clearButtonClicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ShortcutsPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x08,
       3,    1,   37,    2, 0x08,
       4,    0,   40,    2, 0x08,
       5,    0,   41,    2, 0x08,

 // slots: parameters
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void, QMetaType::QKeySequence,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ShortcutsPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ShortcutsPage *_t = static_cast<ShortcutsPage *>(_o);
        switch (_id) {
        case 0: _t->tableItemClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->keyCapLineEditTextChanged((*reinterpret_cast< QKeySequence(*)>(_a[1]))); break;
        case 2: _t->restoreShortcutsButtonClicked(); break;
        case 3: _t->clearButtonClicked(); break;
        default: ;
        }
    }
}

const QMetaObject ShortcutsPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ShortcutsPage.data,
      qt_meta_data_ShortcutsPage,  qt_static_metacall, 0, 0}
};


const QMetaObject *ShortcutsPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ShortcutsPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ShortcutsPage.stringdata))
        return static_cast<void*>(const_cast< ShortcutsPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int ShortcutsPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
