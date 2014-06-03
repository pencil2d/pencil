/****************************************************************************
** Meta object code from reading C++ file 'playbackmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "managers/playbackmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'playbackmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PlaybackManager_t {
    QByteArrayData data[7];
    char stringdata[79];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_PlaybackManager_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_PlaybackManager_t qt_meta_stringdata_PlaybackManager = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 10),
QT_MOC_LITERAL(2, 27, 0),
QT_MOC_LITERAL(3, 28, 3),
QT_MOC_LITERAL(4, 32, 16),
QT_MOC_LITERAL(5, 49, 1),
QT_MOC_LITERAL(6, 51, 26)
    },
    "PlaybackManager\0fpsChanged\0\0fps\0"
    "loopStateChanged\0b\0rangedPlaybackStateChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlaybackManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x05,
       4,    1,   32,    2, 0x05,
       6,    1,   35,    2, 0x05,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool,    5,

       0        // eod
};

void PlaybackManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PlaybackManager *_t = static_cast<PlaybackManager *>(_o);
        switch (_id) {
        case 0: _t->fpsChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->loopStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->rangedPlaybackStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PlaybackManager::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackManager::fpsChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (PlaybackManager::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackManager::loopStateChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (PlaybackManager::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackManager::rangedPlaybackStateChanged)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject PlaybackManager::staticMetaObject = {
    { &BaseManager::staticMetaObject, qt_meta_stringdata_PlaybackManager.data,
      qt_meta_data_PlaybackManager,  qt_static_metacall, 0, 0}
};


const QMetaObject *PlaybackManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlaybackManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PlaybackManager.stringdata))
        return static_cast<void*>(const_cast< PlaybackManager*>(this));
    return BaseManager::qt_metacast(_clname);
}

int PlaybackManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseManager::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void PlaybackManager::fpsChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PlaybackManager::loopStateChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PlaybackManager::rangedPlaybackStateChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
